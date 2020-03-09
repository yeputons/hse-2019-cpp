# Move-семантика — реализация [00:32]
## Категории значений [00:18]
* Ссылки
  * https://en.cppreference.com/w/cpp/language/value_category
  * https://habr.com/ru/post/441742/
* Чистые категории:
  * lvalue — у этого есть имя, нельзя разрушать.
    * Имя переменной: `var`
    * Обращения к полям: `a.m` (тут `a` — lvalue)
    * Обращение по указателю: `p->m` (тут `p` — что угодно)
    * То, что возвращает `T&`: `*p`, `a[i]` (если `a` lvalue для встроенных массивов), `getLval()` (если `int& getLval()`), `static_cast<int&>(x)`
    * Свойства:
      * Можно взять адрес
      * Можно написать "слева"
  * prvalue (как бы были раньше) — имени нет, сейчас помрёт, можно разрушить.
    * Литералы: `42`, `nullptr`
    * То, что возвращает по значению: `a + b`, `getVal()` (если `int getVal();`)
    * Обращения к полям: `a.m` (где `a` — prvalue)
    * `this` (именно сам указатель)
    * Лямбда
    * Свойства:
      * Не полиморфное, мы точно знаем тип.
  * xvalue — новая категория — имя есть, но можно разрушить.
    * То, что возвращает `T&&`: `static_cast<int&&>(x)`, `std::move(x)` (это просто `static_cast` внутри).
    * Обращения к полям: `a.m` (тут `a` — xvalue).
* Смешанные категории:
  * glvalue — то, у чего есть имя. lvalue или xvalue.
  * rvalue — то, из чего можно мувать. prvalue или xvalue.
* Тонкость: `return v;` из функции — в стандарте стоит костыль, чтобы тут вызывался
  move, а не copy, несмотря на то, что `v` — lvalue.

## rvalue-ссылки [00:14]
* Обычная ссылки биндится только к lvalue (к xvalue не могут).
* Константная ссылка биндится к чему угодно: и lvalue, и rvalue.
* C++11 добавляет новый вид ссылки: rvalue-ссылка: `Foo&& x = foo();`
  * Это может биндится только к rvalue (xvalue, prvalue).
* `const Foo&&` — хрень, не пишите так. Забиндится только к rvalue, но move не сработает.
* Конструктор перемещения — просто перегрузка:
  ```
  stack(const stack &other) : data(new ...), ... {}
  stack(/*const, WTF */ stack &&other) /*noexcept*/ : data(std::move(other.data)), cap(std::move(other.cap)), len(std::move(other.len)) {} {
      // А дальше важно занулить cap/len. std::move — это просто смена категории. Для примитивных типов ничем от копирования не отличается.
      other.cap = other.len = 0;
  }
  ```
  * Теперь когда мы пишем `stack s(SOME)` у нас вызывается один из двух конструкторов: наиболее подходящий.
    В зависимости от категории выражения `SOME`.
* После инициализации ссылки нам пофиг, к чему она привязывалась.
  ```
  Foo f;
  Foo &x = f;
  Foo &&y = std::move(f);
  x // lvalue
  x.field // lvalue
  y // lvalue
  y.field // lvalue
  std::move(x).field  // xvalue
  std::move(y).field  // xvalue
  x.field + 10  // prvalue, если field был типа int (а то вдруг operator+ возвращает ссылку).
  ```
* Бывают method ref qualifiers. Раньше у `this` можно было добавлять константность (const qualifier).
  А теперь можно говорить:
  * `void push() & { ... }` — `this` должен быть в категории lvalue (т.е. не временный объект, даже не результат `std::move`).
  * `T top() & { return data[len - 1]; }` — возвращает копию.
  * `T top() && { return std::move(data[len - 1]); }` — мувает объект из стэка. Всё равно стэк сейчас помрёт.
* При этом обычно писать `return std::move(....)` не надо:
  ```
  Foo foo() {
      Foo f;
      // ...
      return std::move(f);  // Тут писать move не надо. Компилятор гарантирует, что либо (N)RVO, либо move. А если написать move, то (N)RVO отрубится, некруто.
  }
  ```

# Специализации шаблонов [00:28]
## Синтаксис для `stack<bool>` [00:06]
```
template<typename T> struct stack { ... }; // Общий случай
template<>
struct stack<bool> { // Для конкретных параметров.
    // Абсолютно независимая реализация.
};
```

## Частичная специализация [00:06]
```
template<typename T> struct unique_ptr {
    T *data;
    ~unique_ptr() { delete data; }
};
template<typename T> struct unique_ptr<T[]> {
    T *data;
    ~unique_ptr() { delete[] data; }
};
```

Работает pattern matching.
Можно зачем-нибудь специализировать как `template<typename A, typename B> struct unique_ptr<map<A, B>> { ... }`.

## Немного type traits [00:08]
Так можно делать вычисления над типами и значениями на этапе компиляции:
```
template<typename T> struct is_reference { constexpr static bool value = false; };
template<typename T> struct is_reference<T&> { constexpr static bool value = true; };
template<typename T> struct is_reference<T&&> { constexpr static bool value = true; };
//
printf("%d\n", is_reference<int&>::value);
```

Это один из способов реализовывать `<type_traits>` (второй будет в 4 модуле, называется SFINAE).

Ещё пример:
```
template<typename U, typename V> struct is_same { constexpr static bool value = false; };
template<typename T> struct is_same<T, T> { constexpr static bool value = true; };
```

А ещё это можно использовать в своих библиотеках.
Например, для сериализации: вы пишете общий случай `template<typename T> struct serializer {};`,
а дальше для каждого типа реализуете `struct serializer { static std::string serialize(const T&); static T deserialize(std::string); }`

## Специализация функций [00:08]
Для функций есть только полная специализация:
```
template<> void swap(Foo &a, Foo &b) { ... }
```
Частичной нет.
Вместо неё предполагается использовать перегрузки, если очень надо:
```
template<typename T> void swap(vector<T> &a, vector<T> &b) { ... }
```
По техническим причинам это не всегда хорошо, правда, но таков факт жизни.

# Прокси-объекты [00:21]
## Для `vector<bool>` [00:15]
Если мы делаем битовую упаковку в `vector<bool>`, то мы больше не можем вернуть `bool&`.

Но надо вернуть что-то такое, чтобы `a[i] = false;` работало.
В C++03 и раньше хорошей идеей были прокси-объекты:

```
template<>
struct vector<bool> {
private:
    struct vector_bool_proxy {
        vector_bool_proxy operator=(bool value) {
            // Установка бита.
        }
        operator bool() {
            return // Чтение бита.
        }
        // А ещё надо operator</operator== для bool по-хорошему, брр...
    };
public:
    vector_bool_proxy operator[](std::size_t i) { return vector_bool_proxy(....); }
    bool operator[](std::size_t i) const { return ....; }
};
```

## Проблемы [00:06]
Начиная с C++11, как только передали в `auto`: `auto x = v[10];`, ой, не `bool`.

Если были forwarding-ссылки: можно использовать хитрость и писать `auto&&`
