## 1. Разбор теста
* Как прошло? Какой формат лучше?
* Дальше смотрим на тест.

---
## 2.1. Долги: перегрузка `operator->`
```c++
template<typename T>
struct unique_ptr {
    T* data;
    // ....

    // Унарный оператор *
          T& operator*()       { return *data; }
    const T& operator*() const { return *data; }

    // "Бинарный" оператор ->, второй аргумент - метод.
    // Так что на самом деле унарный.
          T* operator->()       { return data; }
    const T* operator->() const { return data; }
};
// ....
unique_ptr<std::string> s = ....;
s->size();  // auto x = s.operator->(); x->size();
```
Можно вернуть не `T*`, а что-нибудь с перегруженным `operator->`:
```c++
struct Foo {
    auto operator->() {
        return std::make_unique<std::string>("hello");
    }
} f;
f->size();  // 5; время жизни unique_ptr — до конца full expression (;)
```

---
## 2.2. Долги: rvalue-ref-qualified функторы
У функтора тоже надо сохранить категорию, не только у аргументов.
```c++
struct Foo {
    void operator()(int) & {};       // (1)
    void operator()(int) && {};      // (2)
    void operator()(int) const& {};  // (3); у просто const — конфликт с && и &)
};
Foo f;
const Foo cf;
f(10);      // (1)
Foo()(10);  // (2)
cf(10);     // (3)
```

```c++
template<typename Fn, typename Arg>
decltype(auto) wrap(Fn &&fn, Arg &&arg) {
//    return fn(std::forward<Arg>(arg));
    return std::forward<Fn>(fn)(std::forward<Arg>(arg));
}
// ....
wrap(f, 10);      // (1)
wrap(Foo(), 10);  // (2)
wrap(cf, 10);     // (3)
```             

---
## 2.3. Долги: `friend class`
Были функции-друзья, но для доступа к полям и методам надо объявить
другом целый класс:

```c++
class Bar;
class Foo {
    friend class Bar;
    void privateFunc();
};
class Bar {
    void bar() {
        Foo f;
        f.privateFunc();  // Ok.
    }
};
class Baz {
    void baz() {
        Foo f;
        f.privateFunc();  // Compilation error.
    }
};
```

---
## 2.4. Долги: пример `auto` в определении
```c++
struct HuffmanTree {
    struct Node {};

    Node foo();
};
// До
HuffmanTree::Bar HuffmanTree::foo() { return Node(); }
// После
auto HuffmanTree::foo() -> Node { return Node(); }
```

---
## 2.5. Долги: неявный конструктор присваивания и остальные
Если увлечётесь шаблонами, можно огрести.

Стандарт выделяет:
```c++
class Foo {
    Foo(const Foo&)  // Copy constructor
    Foo(Foo&&)       // Move constructor
    Foo& operator=(Foo&&)       // Move assignment
    Foo& operator=(const Foo&)  // Copy assignment (1)
    Foo& operator=(Foo)         // Copy assignment (2) для copy-and-swap.
};
```
* Если не объявлен копирующий конструктор, то его сгенерирует компилятор.
* Даже если есть другой подходящий конструктор

```c++
class Foo {
    template<typename T>
    Foo(const T&) {}    // (1) не copy constructor.
    // Foo(const Foo&)  // (2) copy constructor.
    // (2) неявно объявлен и определён, приоритет выше (1).
};
```

---
## 2.6. Долги: precompiled header
Если несколько единиц трансляции _начинаются_ одинаково,
можно запомнить промежуточное состояние компилятора для скорости.

```c++
// a.cpp
#include "precompiled.h"
#include <rare_lib_1.h>
int main() { .... }
// b.cpp
#include "precompiled.h"
#include <rare_lib_2.h>
void doB() { .... }
```

* Обычно требуется выделить общий префикс в файл `precompiled.h`/`stdafx.h`
  и подключать его первым.
  * Сюда же `<bits/stdc++.h>`
* Разные компиляторы действуют по-разному и не совместимы.
* Требуется идеальное совпадение ключей и версий компилятора.

```bash
# g++
g++ precompiled.h  # Создаст precompiled.h.gch и положит рядом с precompiled.h
g++ a.cpp b.cpp    # Автоматически найдёт precompiled.h.gch и воспользуется
# clang++
clang++ precompiled.hpp  # Создаст свой precompiled.h.gch
clang++ -include-pch precompiled.hpp.gch a.cpp b.cpp  # Будет двойное включение
```

---
## 2.7. Долги: Pimpl
* Пусть есть большой класс `GodClass` с кучей приватных методов и полей,
  от которого все зависят.
* При любом изменении `GodClass` надо пересобирать весь проект.
* Решение: [идиома PImpl](https://en.cppreference.com/w/cpp/language/pimpl) (pointer-to-implementation)
  * Выносим реализацию в отдельный класс, скрытый в `.cpp`
  * А публичный интерфейс хранит указатель на "реализацию".
  * Чуть больше тормозов и памяти, зато компилировать проще.

```c++
// god_object.hpp
class GodObject {
    GodObject();
    int publicFoo();
private:
    struct impl;
    std::unique_ptr<impl> pImpl;  // Надо ещё с const аккуратно.
};
// god_object.cpp
struct GodObject::impl {
    int privateInt1 = 10;
    int privateInt2 = 20;
    int foo() { return privateInt1; }
};
GodObject::GodObject() : pImpl(std::make_unique<impl>()) {}
int GodObject::publicFoo() {
    return pImpl->foo() + pImpl->privateInt2;
}
```

---
## 2.8. Долги: `thread_local`
Иногда хочется завести переменную, глобальную только для текущего потока.
Например, какой-нибудь кэш, чтобы не добавлять mutex'ов.

Есть модификатор `static` по аналогии с `thread_local`:

```c++
thread_local int x = 10;  // Можно и внутри функции.
void foo() {
    std::cout << x << "\n";
    x++;
    std::cout << x << "\n";
}
//
std::thread t1(foo);  // 10, 11
std::thread t2(foo);  // 10, 11
```

---
## 3.1. Variadic template: синтаксис
Можно определить _variadic template_ — шаблон класса, у которого
в параметрах есть [_template parameter pack_](https://en.cppreference.com/w/cpp/language/parameter_pack):

```c++
template<typename A, int N, typename ...Ts>
struct Foo {
     static constexpr inline std::size_t M = sizeof...(Ts);
     std::tuple<Ts...> xs;
     // Ts... xs;  // Так нельзя :(
};
Foo<int, 10> f1;                                        // M = 0
Foo<int, 10, int, int> f2;                              // M = 2
Foo<int, 10, int, Foo<int, 10, char>, std::string> f3;  // M = 3

template<int ...Ns>
struct Bar {
    static constexpr inline std::size_t M = sizeof...(Ns);
};
Bar<> b1;            // M = 0
Bar<10, 20, 30> b1;  // M = 3
```

* Template parameter pack может быть только последним,
  чтбы можно было жадно понять, кто к нему относится.
* Мнемоника: при объявлении `...` пишется слева, при разворачивании
  (pack expansion) справа.

---
## 3.2. Variadic template: pack expansion для типов
```c++
int func() {}

template<typename ...Ts>
struct Foo1 {
    template<Ts ...Values> struct Bar {};
};
Foo1<int, int(*)()>::Bar<10, func> x;

// С C++17
template<auto Value> struct Foo2 {};
Foo2<10> y;

template<auto ...Values> struct Foo3 {};
Foo3<10, func, func> z;
```

---
## 3.3. Variadic template: pack expansion для базовых классов
```c++
struct Foo {
    Foo(int);
    int func(int);   // (1)
};
struct Bar {
    Bar(int);
    int func(char);  // (2)
};
template<typename ...Ts>
struct Hybrid : Ts... {
    Hybrid() : Ts(10)... {}
    using Ts::func...;  // С C++17
};
Hybrid<Foo, Bar> h;
// ....
h.func(10);   // (1)
h.func('x');  // (2)
```

---
## 3.4. Обращение к элементам
**Получить элемент по номеру нельзя**.
Только рекурсия [частичных специализаций](https://en.cppreference.com/w/cpp/language/partial_specialization):

```c++
template<typename .../*Ts*/> struct head {};
template<typename Head, typename ...Tail>
struct head<Head, Tail...> {
    using type = Head;
};
```

* В специализациях можно сколько угодно любых parameter pack.
* При этом если разворачиваем parameter pack, то он должен идти последним.
  * Чтобы компилятор мог жадно проверить, подходит ли специализация.

```c++
// Окей
template<typename, typename, typename>
struct Foo {};
template<typename ...As, typename T>
struct Foo<T, std::tuple<As...>, T> {};
// Не окей
template<typename...> struct Bar {};
template<typename ...Args> struct Bar<Args..., int> {};  // :(
```

---
## 3.5.1. Реализация своего `std::tuple`
Надо писать рекурсивно, как односвязный список на Haskell:
```haskell
data List a = Nil | Const a (List a)
```

```c++
template<typename ...Args> struct tuple {};
template<typename T, typename ...Args>
struct tuple<T, Args...> {
    T head;
    tuple<Args...> tail;  // Можно наследование, а не поле.
};
// ....
tuple<int, std::string> x;
x.head            // int
x.tail            // tuple<std::string>
x.tail.head       // std::string
x.tail.tail       // tuple<>
x.tail.tail.head  // compilation error
```

Вспомогательная метафункция:
```c++
template<typename ...Args> struct tuple_size {};
template<typename ...Args>
struct tuple_size<tuple<Args...>>
    : std::integral_constant<std::size_t, sizeof...(Args)> {};
```

---
## 3.5.2. Реализация `tuple_element` и `get<>`
```c++
template<std::size_t, typename /*Tuple*/> struct tuple_element;
// Базовый случай.
template<typename Head, typename ...Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;
};
// Основной случай.
template<std::size_t I, typename Head, typename ...Tail>
struct tuple_element<I, tuple<Head, Tail...>> : tuple_element<I - 1, tuple<Tail...>> {
};
// ....
tuple_element<0, tuple<int, string>> = int
tuple_element<0, tuple<int, string>> = tuple_element<1, tuple<string>> = string
```

`get<>` тоже рекурсивно:
```c++
template<std::size_t I, typename ...Ts>
auto get(const tuple<Ts...> &tuple) {  // auto& / tuple&
    // Можно if constexpr, можно специализация для I = 0.
    if constexpr (I == 0) return tuple.head;
    else                  return get<I - 1>(tuple.tail);
}
// ....
tuple<int, string> x;
get<0>(x) == x.head  // int
get<1>(x) == get<0>(x.tail) == x.tail.head  // string
```

---
## 3.6. Одновременное разворачивание
Слева от `...` должен находится **pattern**, он разворачивается целиком:

```c++
template<typename, typename> struct ZipTuple;
template<typename ...As, typename ...Bs>
struct ZipTuple<tuple<As...>, tuple<Bs...>> {
    using type = tuple<pair<As, Bs>...>;
}
// ....
ZipTuple<tuple<int, char>, tuple<char, string>>::type =
    tuple<
        pair<int, char>,
        pair<char, string>
    >
```

* Если внутри одного pattern есть несколько parameter pack, они должны быть
  одинакового размера, тогда разворачиваются "параллельно".
* Иначе ошибка компиляции.

Написать декартово произведение нельзя, надо эмулировать руками.

---
## 3.7.1. Возврат parameter pack невозможен
```c++
template<typename .../*Ts*/> struct tail {};
template<typename Head, typename ...Tail>
struct tail<Head, Tail...> {
    using types = Tail...;  // Нельзя: внутри структуры могут быть только типы.
};
```
Единственное решение: создать вспомогательный тип:
```c++
template<typename...> struct type_list {};
template<typename .../*Ts*/> struct tail {};
template<typename Head, typename ...Tail>
struct tail<Head, Tail...> {
    using type = type_list<Tail...>;
};
template<typename ...Ts> using tail_t = typename tail<Ts...>::type;
```
Но это сложно использовать:
```c++
template<typename> struct type_list_to_tuple {};
template<typename ...Ts> struct type_list_to_tuple<type_list<Ts...>> {
    using type = tuple<Ts...>;
};
typename type_list_to_tuple<tail_t<int, string>>::type = tuple<string>
```

---
## 3.7.2. Общее решение для возвращата
Попытка "в лоб": можно передать `tuple` как параметр:
```c++
// Заглушка
template<template<typename...> typename /*F*/, typename /*TypeList*/>
struct apply_type_list;
// Реализация
template<template<typename...> typename F, typename ...Ts>
struct apply_type_list<F, type_list<Ts>> {
    using type = F<Ts...>;
};
typename apply_type_list<tuple, type_list<int, string>>::type == tuple<int, string>
```

Что-то похожее [уже сделано в Boost.Hana](https://stackoverflow.com/a/38009838/767632).
Только там синтаксис другой, лучше начать с мануала.
