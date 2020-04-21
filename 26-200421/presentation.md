## План
1. Тонкости из домашек:
  1. If with init statement.
  1. Передача параметров в функции и умные указатели.
  1. Как объявлять константы.
1. `auto [a, b] = my_set.insert(..)`
  * Нам потребуется `std::tuple<int, int, int>`
  * Удобный сахар, но с подвохами, надо знать.
1. RTTI (Run-Time Type Information)
  * Через него работает `dynamic_cast`.
  * Напишете на практике свой `std::any`.
1. Unqualified lookup для функций, ADL.
  * Почему `std::operator<<`, но пишем `using std::cout; cout << a`?

---
## 1.1. If with init statement (C++17)
<!-- На лекции, чтобы попало на экзамен. -->

До:
```c++
map<int, string> m = ....;
{
    typename map<int, string>::iterator it = m.lower_bound(10);
    if (it != m.end()) cout << it->second << '\n';
    else               cout << "Not found\n";
}  // После этой скобки `it` недоступен
```

После (строго лучше): <!-- обвести два куска, проверсти аналогию с `for` -->
```c++
map<int, string> m = ....;
if (auto it = m.find(10); it != s.end()) {
    cout << it->second << '\n';
} else {
    cout << "Not found\n";  // it тут доступен.
}  // После этой скобки `it` недоступен.
```

Аналогично полезно с `.find()` и `dynamic_cast<T*>`, смотри [`25-200416p/problems/01-basic/06-dynamic-cast.cpp`](https://github.com/yeputons/hse-2019-cpp/blob/26daebb971d2c5b6f8f995e7aa40fdf9ebfce486/25-200416p/problems/01-basic/06-dynamic-cast.cpp)

---
## 1.2.1. Передача параметров в функции
Смотри [GotW 91](https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/) (Guru of the Week, автор — Herb Sutter).

* Практически всегда по значению, `&&` и умные указатели не нужны:
    ```c++
    void foo(vector<int> foo_data) { sort(foo_data.begin(), foo_data.end(); .. };
    int main() {
        vector<int> data = {1, 2, 3, 4, 5};
        foo(data);  // Тут скопируем, вектор ещё нужен.
        data.emplace_back(6);  // {1, 2, 3, 4, 5, 6}
        foo(std::move(data));  // Разрешили не копировать.
        data.clear();  // Вектор мог остаться непустым, не определено moved-from.
    }
    ```

* Если хотим отдать что-то с именем (даже `&&`) — пишем `std::move`:
    ```c++
    template<typename T> struct MyVector {
        vector<T> data;
        MyVector(vector<T> data_) : data(std::move(data_)) { /* data_ = ?? */ }
        void push_back(T &&value) { data.emplace_back(std::move(value)); }
    };
    ```

---
## 1.2.2. Параметры только для чтения/для записи
* Если нам вcегда передают объект, а мы его только читаем:
  ```c++
  void printVector(const vector<int> &data);
  ```

* Если иногда не передают — `std::optional` (не указатель!):
  ```c++
  void maybePrintVector(const std::optional<vector<int>> &data);
  ```

* Если нам всегда нужно передать наружу объект (не указатель!):
  ```c++
  void readAndAppendToVector(vector<int> &data);
  ```
  _Осторожно_: а что, если `data` исходно непуст? Не надо думать — верните по значению.

* Если пользователь не всегда хочет ответ — указатель.
  <!-- Указатель здесь лучше ссылки только потому что в нём может быть `nullptr`. В указатель запишем ответ. Владеет им вызывающий. -->
  ```c++
  void readAndMaybeAppendToVector(vector<int> *const data);
  ```

---
## 1.2.3. Rvalue-ссылки в параметрах
Не нужно, если объект умеет только `move` ([`unique_ptr`](https://stackoverflow.com/a/8114913/767632)):
```c++
void foo(unique_ptr<Foo> x /* unique_ptr<Foo> &&x */);
unique_ptr<Foo> bar;
foo(bar);             // Не скомпилируется в обоих случаях.
foo(get_bar());       // Скомпилируется в обоих случаях.
foo(std::move(bar));  // Скомпилируется в обоих случаях.
```
<!--
Это может быть чуть менее эффективно в общем случае: тут
мы вызываем лишний конструктор перемещения и деструктор.
Для `unique_ptr` всё равно.
-->

Обычно не нужно, если объект можно копировать:
```c+++
void foo(Foo &&x);
Foo bar;
foo(bar);             // Не компилируется.
foo(Foo(bar));        // Надо явно копировать, но зачем это требовать?
foo(std::move(bar));  // Явно мувать можно всегда.
```

Иногда `&&` нужно для оптимизаций:

```c++
void push_back(const T&);  // 1 copy вместо 1 copy + 1 move + 1 dtor.
void push_back(T&&);       // 1 move вместо 1 move + 1 move + 1 dtor.
```

---
## 1.2.4. Умные указатели в параметрах
Умный указатель — пара `(данные, владение)`.
В параметрах — только если нам важно, как именно им владеет __вызвавший__.
Обычно неважно.

* `const unique_ptr<T>&` — лучше заменить на `T&` (`const` явно исчез). <!-- константен только сам указатель; а вдруг у нас вообще объект на стеке? Тоже уникальный владелец -->
  * Это не семантика &laquo;у объекта один владелец&raquo; — стэк, поле...
* `unique_ptr<T>&` — почти никогда <!-- пример: swap, за деталями — в GotW -->
* `const shared_ptr<T>&` — почти никогда <!-- пример: когда иногда хотим себе скопировать, за деталями — в GotW  -->
* `shared_ptr<T>&*` — почти никогда

```c++
// Окно нельзя копировать, важно считать ссылки и владеть совместно.
void addToAnotherDesktop(shared_ptr<Window> window) {  // По значению.
    recentlyMovedWindows.emplace_back(window);         // Можем копировать.
    myWindows.emplace_back(std::move(window));         // Можем мувать.
}
```
```c++
Node(Node left_, Node right_)
    : left(make_unique<Node>(std::move(left_))), .... {}
// Оптимизация: всегда оборачиваем в `unique_ptr`, давайте сразу его возьмём.
Node(unique_ptr<Node> left_, unique_ptr<Node> right_)  // Без &&
    : left(std::move(left_)), right(std::move(right_)) {}
```

---
## 1.3.1. Как объявлять константы вне классов
<!-- Если внутри `.cpp`/`.h`, не внутри классов: -->

Лучше с `constexpr` вместо `const`: https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rconst-constexpr
```c++
constexr int N = 10000;
int arr[N];
```
Это не форсирует вычисление в compile-time, но требует возможность.

Не прокатит со сложными объектами (`std::string`) и поделом: проблемы с порядком инициализации
между разными translation unit.

```c++
// my.h
const std::string FileOption = "--file";  // const => internal linkage!
// a.h
#include "my.h"
.. &FileOption ..  // (1), отличается от (2), и ладно?
// b.cpp
#include "my.h"
.. &FileOption ..  // (2), отличается от (1), и ладно?
```

Можно `extern` и думать про ODR, но лучше `constexpr char FileOption[]`.

---
## 1.3.2. Статические константы-члены класса
```c++
// foo.h
struct Foo {
    static const int N = 10;  // external linkage!
    static constexpr char Name[] = "NAME";  // external linkage до C++17
};
// foo.cpp


.. Foo::N ..          // ok
.. Foo::Name[0] ..    // ok
.. &Foo::N ..         // undefined reference?
.. &Foo::Name[0] ..   // undefined reference?
}
// main.cpp
.. Foo::N ..          // ok
.. Foo::Name[0] ..    // ok
.. &Foo::N ..         // undefined reference?
.. &Foo::Name[0] ..   // undefined reference?
```

Как с функциями или глобальными переменными: мы только _объявили_ константу
(несмотря на наличие инициализатора).

---
## 1.3.3. Определение констант
```c++
// foo.h
struct Foo {
    static const int N = 10;  // external linkage!
    static constexpr char Name[] = "NAME";  // external linkage до C++17
};
// foo.cpp
const int Foo::N;           // Инициализировать уже не надо.
constexpr char Foo::Name[]; // Слово `static` не нужно.
.. Foo::N ..         // ok
.. Foo::Name[0] ..   // ok
.. &Foo::N ..        // ok
.. &Foo::Name[0] ..  // ok
}
// main.cpp
.. Foo::N ..         // ok
.. Foo::Name[0] ..   // ok
.. &Foo::N ..        // ok
.. &Foo::Name[0] ..  // ok
```

Обязаны определить, причём ровно один раз (ODR).
Не всегда, но лучше не рисковать (IFNDR хуже UB).

---
## 1.3.4. Идеальное объявление констант
С C++17 можно добавить слово `inline` к переменным и константам
([TotW&nbsp;168](https://abseil.io/tips/168) — Tip of the Week), как для функций:

* Все объявления становится и определениями.
* Разрешается несколько определений, если они совпадают по токенам
  * Иначе **IFNDR** ill-formed, no diagnostics required), UB навсегда.

Итого: **почти всегда `constexpr`, а в заголовках — всегда `inline`**.

```c++
struct Foo {
    static inline const std::string BAD = "Use constexpr";
    static inline constexpr char Name[] = "NAME";
};
inline const std::string BAD = "Really, use constexpr";
inline constexpr char Name[] = "NAME";
```

<!--
Из `static constexpr` в C++17 следует `inline`, но я рекомендую всё равно писать.
-->

---
## 2.1.1. `std::tuple`
Аналог `std::pair`, но хранит произвольное количество элементов
(гетерогенный список).

```c++
std::tuple<int, vector<int>, string> t(10, vector<int>(2), "foo");
auto t2 = std::make_tuple(10, vector<int>(2), "foo");  // t == t2
assert(t == t2);
int a = std::get<0>(t);  // Должна быть константа времени компиляции, циклов нет.
string c = std::get<2>(t);
```

Есть неявная конверсия из `pair<>` и `tuple_cat` (пригодится в метапрограммировании):
```c++
std::pair<int, string> p(10, "foo");
std::tuple<int, string> t = p;  // Неявная конверсия pair --> tuple.
auto tt = std::tuple_cat(t, t);
```
Можно узнавать тип элемента и размер на этапе компиляции:
```c++
std::tuple_element_t<0, decltype(tt)> x = std::get<0>(tt);
static_assert(std::tuple_size_v<decltype(tt)> == 4);
```

---
## 2.1.2. Интересности с `std::tuple`
<!-- Можно пропустить -->
Пригодится нам потом в метапрограммировании:
```c++
void foo(int a, string b);
// ....
auto t = std::make_tuple(10, "hello");
std::apply(foo, t);  // foo(10, "hello");
```

Можно хранить ссылки (в `pair` тоже):
```c++
int a = 10; string b = "foo";
std::tuple<int&, string&> t(a, b);
t = std::make_tuple(20, "bar");  // a == 20, b == "bar"
assert(a == 20);
assert(b == "bar");
```
С этим есть эффекты, аккуратно разбирать пока не будем.

Можно почти как в Python:
```c++
// std::tuple<int, string> bar() { return {30, "baz"}; }
std::tie(a, b) = bar();  // a == 30, b == "baz"
```

---
## 2.2.1. Structured binding — базовое
С С++17 можно почти совсем как в Python:

```c++
std::pair<int, string> p(10, "foo");
auto [a, b] = p;  // a == 10, b == "foo"
b += "x";  // b == "foox", p.second == "foo"
```

К `auto` можно добавлять `const`/`&`/`static`:

```c++
auto& [a, b] = p;  // a == 10, b == "foo"
b += "x";  // b == p.second == "foox"
```

* Есть direct initialization: `auto [a, b](p);`.
* Есть list initialization: `auto [a, b]{p};`.
* Указать тип отдельных `a`/`b` нельзя.
* Нельзя вкладывать: `auto [[a, b], c] = ...`.
* Нельзя в полях.
* Происходит на этапе компиляции: можно с массивами, но не с векторами.
* Также работает с очень простыми структурами.

---
## 2.2.2. Structured binding — применения
Удобно получать значения `pair` из `.insert`.
```c++
map<int, string> m = ....;
if (auto [it, inserted] = m.emplace(10, "foo"); inserted) {
    cout << "Inserted, value is " << it->second << '\n';
} else {
    cout << "Already exists, value is " << it->second << '\n';
}
```

Удобно итерироваться по `map`.
```c++
for (const auto &[key, value] : m) {
    cout << key << ": " << value << '\n';
}
```

---
## 2.3.1. Как работает
```c++
auto [key, value] = *m.begin();
/*                  ^^^EXPR^^^ */
```
превращается в
```c++
// 1. Объявляем невидимую переменную ровно так же
//    Для примера тут copy initialization.
auto e = *m.begin();  // map<int, string>::value
                      // pair<const int, string>
using E = pair<const int, string>;
// 2. Проверяем количество аргументов.
static_assert(std::tuple_size_v<E> == 2);
// 3. Объявляем элементы.
std::tuple_element_t<0, E> &key   = get<0>(e);  // Или e.get<0>()
std::tuple_element_t<1, E> &value = get<1>(e);  // Или e.get<1>()
```

* На самом деле `key` и `value` — ссылки в невидимый `e`.
* Время жизни такое же, как у `e`.
* Костантность и ссылочность получаем от `tuple_element_t`.
  * В частности, `const auto &[a, b] = foo()` продлит жизнь временному
    объекту.

---
## 2.3.2. Подробности structured binding
Поддерживаются три формы привязки:

1. Если массив известного размера:
   ```c++
   Foo arr[3];
   auto [a, b, c] = arr;
   // превращается в
   auto e[3] = { arr[0], arr[1], arr[2] };
   Foo &a = e[0], &b = e[1], &c = e[2];
   ```
* Если не массив, то `tuple_size<>`, `get<>`...
  * Можно предоставить для своего типа, но надо думать про `get`
    от `const` (deep или shallow?) и
    [прочие тонкости](https://stackoverflow.com/questions/61340567).
* Иначе пробуем привязаться ко _всем_ нестатическим полям.
  ```c++
  struct Good { int a, b; }
  struct GoodDerived : Good {};

  struct BadPrivate { int a; private: int b; }  // Приватные запрещены.
  struct BadDerived : Good { int c; }  // Все поля должны быть в одном классе.
  ```

---
## 2.3.3. Тонкости structured binding
* В зависимости от `auto`/`auto&`/`const auto&` и инициализатора у нас получаются немного разные типы.
  * `auto&` попробует привязать ссылку.
  * `const auto&` продлит жизнь временному объекту.
  * `auto` всегда скопирует объект целиком, а не просто его кусочки.

Если внутри объекта лежали ссылки, то может [сломаться время жизни](https://stackoverflow.com/a/51503253/767632):
```c++
namespace std {
    std::pair<const T&, const T&> minmax(const T&, const T&);
}
auto [min, max] = minmax(10, 20);  // Только копирование значений?
// перешло в
const pair<const int&, const int&> e = {10, 20};
// Сам `e` — не временный, поэтому продления жизни нет.
// e.first и e.second ссылаются на уже умершие 10 и 20.
const int &min = e.first;   // Oops.
const int &max = e.second;  // Oops.
```

Рекомендация: осторожно с функциями, которые возвращают ссылки.
С ними лучше `std::tie`.

---
## 3.1.1. RTTI и `dynamic_cast`
* Run-Time Type Information для полиморфных типов
  * Хотя бы одна виртуальная функция.
* `dynamic_cast` смотрит на самом деле на RTTI.

```c++
struct Base { virtual ~Base(); int x; };
struct Derived : Base { int y; };

Base *b = ...;
Derived *d = dynamic_cast<Derived*>(b);
```

Может выглядеть так (а может и по-другому):
![RTTI probable layout](RTTI.svg)

---
## 3.1.2. RTTI и `typeid(..)`
```c++
#include <typeinfo>
// ....
Derived d;
Base *b = &d;
const std::type_info &info_b = typeid(*b);  // Всегда(!) по const&
assert(info_b == typeid(d));
assert(info_b == typeid(Derived));
assert(info_b != typeid(Derived*));
```

При этом ничего про тип узнать нельзя: reflection отсутствует, поля не перечислить,
родителей не перечислить...

Но можно делать безопасный type erasure вместо `void*`:
```c++
void print(const std::any &a) {
    if (auto *pInt = std::any_cast<int>(&a); pInt)
        cout << "int: " << *pInt << '\n';
}
// ....
print(10);    // int: 10
print("foo"); //
```

---
## 3.1.3. `type_info::name()`
```c++
template<class T> struct X {};
// ....
char const *name = typeid(X<int>).name();
std::cout << name << '\n';  // 1XIiE ???
```

`type_info::name()` по стандарту возвращает _что угодно_.

* Может совпадать у разных типов
* Может отличаться от запуска к запуску
* Иногда помогает расшифровать при помощи `boost::core::demangle`
    * `sudo apt install libboost-all-dev` — в Boost очень много полезного.

```c++
#include <boost/core/demangle.hpp>
// ....
std::cout << boost::core::demangle(name) << '\n';  // X<int>
```

---
## 3.1.4. `type_index`
* `type_info` нельзя положить в контейнер:
  ```c++
  set<const std::type_info&> known_types;  // Не компилируется.
  ```
  * Нельзя копировать и перемещать, только по `const type_info&`.
  * Нет `std::hash<>` (есть `.hash()`) и нет `operator<` (есть `.before()`).
* По `name()` нельзя, он может совпадать у разных типов.
* По адресам `type_info` нельзя: про них никаких гарантий нет.
* Есть обёртка `std::type_index` (можно и самому написать):
  ```c++
  #include <typeindex>
  // ....
  set<std::type_index> known_types;
  known_types.insert(typeid(int));
  known_types.insert(typeid(short));
  known_types.insert(typeid(std::type_index));
  ```

---
## 3.1.5. Тонкости `typeid`
Всегда отбрасывает ссылки и верхнюю константность.
Три режима:

* От типа — на этапе компиляции:
  ```c++
  const std::type_info &t1 = typeid(Base);
  const std::type_info &t2 = typeid(Base*);
  assert(typeid(Base) == typeid(const Base&));
  ```
* От неполиморфного выражения — на этапе компиляции:
  ```c++
  struct A { int x; };
  struct B : A { int y; };
  A foo();  // Никогда не вызывается.
  // ....
  assert(typeid(foo()) == typeid(A));
  B b;
  const A &bref = b;
  assert(typeid(bref) == typeid(A));
  ```
* От полиморфного — вычисляет и смотрит тип.
  * Если получил `nullptr`, то кидает `std::bad_typeid`.

---
## 3.1.6. Использование RTTI
Применения:

* `dynamic_cast<>`
* Type erasure: сохраняем любые копируемые объекты в `std::any` (вместо `void*`)
  и используем `any_cast` без UB.

При этом исторически стоит дорого, много не умеет => редко используется => бывают баги в компиляторах и занимает много памяти.

* Во встраиваемом программировании отрубается `-fno-rtti`
* Можно реализовать самому как библиотеку: смотри [Boost.TypeIndex](https://www.boost.org/doc/libs/1_72_0/doc/html/boost_typeindex.html).
