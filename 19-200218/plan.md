# Мелочи шаблонов [00:25]
## typedef, using [00:04]
* Инстанцированный шаблон — это тип, можно сделать `typedef`: `typedef vector<int> vi;`
* Неинстанцированный — нельзя: `typedef vector v;` не работает.
* Два костыля:
  * `#define vector v` — сразу проблема, потому что разные namespaces, имена переменных...
  * `template<typename T> struct v : vector<T> {};` — это всё-таки разные типы получаются, надо добавлять какие-то операторы преобразования из `v` в `vector`.
* Нормальный способ: `template<typename T> using v = vector<T>`, называется alias template.
  * Можно и хитрое: `template<typename T>

## Много параметров, параметры по умолчанию [00:03]
```
template<typename T, typename U = void, typename V = int>
struct T { ... };
T<char> x; // T<char, void, int>
T<char, char> y; // T<char, char, int>
T<char, char, char> z; // T<char, char, char>
```
С функциями тоже работает.
Только там ещё есть автовывод типов.

## Параметры-значения [00:03]
```
template<typename T, std::size_t N /* = 10 */>
struct my_array {
    T arr[N];
    std::size_t size() const { return N; }
};
my_array<int, 10> arr;
```
Можно указывать простые параметры: целые числа (`bool` тоже), любые указатели и ссылки на статические переменные.
Нельзя указывать `std::string` и сложные типы.
С С++20 разрешили какие-то структуры, вещественные числа.

Снова возникают тонкости с парсингом: `foo<0>1>` надо писать как `foo<(0>1)>`.

## Параметры-шаблоны [00:05]
```
template<typename T, template<typename> typename Container = std::vector>
struct priority_queue {
    Container<T> c;
    ...
};
priority_queue<int>  // priority_queue<int, std::vector>
priority_queue<int, priority_queue>  // struct priority_queue<int, priority_queue> { priority_queue<int> c; }
```

### Шаблонные методы в шаблонных классах [00:05]
```
template<typename T>
struct shared_ptr {
    Storage *s;
    shared_ptr(T *data);
};
struct Base {};
struct Derived : Base {};
void foo() {
    shared_ptr<Derived> d = ...;
    shared_ptr<Base> b = d;  // Упс, несовместимые типы.
}
```

Решение:
```
template<typename T>
struct shared_ptr {
    ...    
    template<typename U>
    shared_ptr(shared_ptr<U> p);
};
```

При инстанцировании проверит, что так можно инициализировать указатели.
В частности, не разрешит из `Derived*` получить `Base*` или `int`.

Можно не только с конструкторами, но и с методами.

## Вложенные типы/значения и слова `typename`/`template` [00:05]
```
template<typename T> struct A { ... };
template<typename T> struct B {
    void foo() {
        /* typename */ A<T>::x * y;
    }
};
```
Это либо умножение, либо объявление переменной типа `T::x`.
Пока не посмотрим в шаблон — не узнаем.
А ещё бывают специализации (будут потом), так что в зависимости от `A` это может быть разное.

Некоторые компиляторы требуют, чтобы вы явно писали слово `typename` перед заким зависимым типом:
`typename A<T>::x * y`, иначе они будут трактовать `x` как переменную.
А если написать это как поле класса, то выдадут ошибку.

Более сложный пример:
```
template<typename T> struct A { int x; };
template<typename T> struct B {
    template<typename U> using C = A<U>;
};
template<typename T> struct D {
    typename B<T>::template C<T>::x * y;
};
```

Аналогичная проблема может возникнуть при вызове шаблонных функций, которым
хотим явно задать параметры:
```
template<typename T> struct Foo {
    template<typename U> void foo();
};
template<typename T>
void bar() {
    Foo<T> x;
    x./*template */foo<char>();
}
```

## Автовывод параметров типа (C++17) [00:05]
```
template<typename T1, typename T2> struct pair { T1 first; T2 second; }
template<typename T1, typename T2> pair<T1, T2> make_pair(T1 a, T2 b) {
    return pair<T1, T2>(a, b);
}
template<typename T1, typename T2>
foo(pair<T1, T2> x);
void bar() {
    foo(pair<int, int>(10, 10)); // ок
    foo(make_pair(10, 10));  // ок, автовывод
    foo(pair(10, 10)); // не ок до C++17, неизвестен тип.
}
```
Начиная с C++17 есть Class template argument deduction, который пытается
перебрать конструкторы и решить уравнение на типы класса.
Можно добавлять и свои уравнения, но мы не будем.

# Правила вывода типов [00:20]
## Где случается [00:07]
CppCon 2014: Scott Meyers "Type Deduction and Why You Care"
(первые полчаса; забить на `decltype`, `T&&`/universal references)

В C++03 был только автовывод параметров у функций, просто работало.

* Дано: `template<typename T> void foo(ТИП x);` и вызываем `foo(value);`.
  Надо найти: `T = ?`, `ТИП = ?`.
  Они могут отличаться.
  Например, если `ТИП=vector<T>` или `ТИП = T&` или `ТИП=vector<T*>*`.
* В C++11 появляются `auto` переменные:
  ```
  auto x = max(10, 20);
  map<int, int> my_map;
  auto it = my_map.begin();
  ```
  Здесь слово `auto` играет роль `T`.
* Также в C++11 появляются лямбды, у них надо возвращаемое значение:
  ```
  auto lambda = [&](int a, int b) /* -> bool*/ { return a * x < b * x; }
  ```
* А в C++14 появляется вывод типов для любых функций: вывели для первого `return`.
  Дальше они все должны совпасть.
  Если `return`'ов нет, то `void`.
  ```
  auto sum(int n) {
      if (n == 0) return 0;
      else return n + sum(n - 1);  // Окей, тип уже знаем. Поменять местами не окей.
  }
  ```
* Ещё в C++14 появляются lambda init capture:
  ```
  int x = 10, y = 20;
  auto lambda = [z = x + y](int a) { return a * z; };
  assert(lambda(-1) == -30);
  ```

## Общая схема [00:02]
Идём рекурсивно по `ТИП` сверху вниз.
Будем разбирать такой пример:
```
template<typename T> void foo(T &x); // ТИП=T&
int x;
const int cx = x;
const int &rx = x;
```

## По значению [00:05]
Если видим `НЕЧТО` без ссылок и констант: хотим копию значения.

* Если `value` — ссылка, убери это.
* Если `value` — `const` или `volatile` на верхнем уровне, убери это.
* То, что осталось — делай pattern matching с `НЕЧТО`.

```
auto v7 = x; // T = int, ТИП = int
auto v8 = cx; // T = int, ТИП = int
auto v9 = rx; // T = int, ТИП = int
```

## Для ссылок и указателей [00:06]
Если видим `НЕЧТО*` или `НЕЧТО&`:

* Если `value` — ссылка, убери это.
* Сделай pattern-matching типа `value` с `НЕЧТО`, выясни `T`.
  Оно никогда не ссылка.

```
foo(x); // T = int, ТИП = int&
foo(cx); // T = const int, ТИП = const int&
foo(rx); // T = const int, ТИП = const int&
```

`auto`-переменная работает так же:
```
auto &v1 = x; // T = int, ТИП = int&
auto &v2 = cx; // T = const int, ТИП = const int&
auto &v3 = rx; // T = const int, ТИП = const int&
const auto &v4 = x; // T = int, ТИП = const int&
const auto &v5 = cx; // T = int, ТИП = const int&
const auto &v6 = rx; // T = int, ТИП = const int&
```

При этом `const` лежит "за" ссылкой:
```
template<typename T> void bar(const T &x);
bar(x); // T = int, ТИП = const int&
bar(cx); // T = int, ТИП = const int&
bar(rx); // T = int, ТИП = const int&
```

На засыпку:
```
int *a;
int * const b;
const int * c;
const int * const d;
auto v1 = a;  // T=int*, ТИП=int*
auto v2 = b;  // T=int*, ТИП=int*
auto v3 = c;  // T=const int*, ТИП=const int*
auto v4 = d;  // T=const int*, ТИП=const int*
```

# Шаблонный стэк с гарантиями исключений [00:35]
