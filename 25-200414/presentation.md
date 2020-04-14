## `make_shared`
Если пишем `std::shared_ptr ptr(new T());`, то в куче есть два объекта:
<pre>
┌───┐  ┌───────────────┐
│ T │  │shared_ptr_data│
└───┘  └───────────────┘
</pre>
Если пишем `std::make_shared<T>()`, то выделяем один блок рядом:
<pre>
┌───┬───────────────┐
│ T │shared_ptr_data│
└───┴───────────────┘
</pre>
* Меньше выделений памяти
* Больше локальнсть, быстрее работает.

---
## `string_view`, `span`
```с++
void print_line(const char *);        // 1
void print_line(const std::string&);  // 2
std::string s;
```

1. Надо делать `print_line(s.c_str());` и делать C-style строку
2. В строчке `print_line("123")` лишнее выделение на куче

Для невладеющего доступа к отрезкам лучше использовать:
```с++
struct string_view {  // C++17
    const char *data; size_t length;
    string_view substr(size_t pos = 0, size_t count = npos) const;
};
template<typename T>
struct span { const T *data; size_t length; }  // C++20
```

Теперь можно работать без лишних копирований:
```с++
std::vector<std::string_view> split(std::string_view str, std::string_view sep);
```

---
## Детали реализации `vector<>`
### Выделение памяти
* `std::aligned_alloc(std::size_t alignment, std::size_t size)`
  работает с произвольным `alignment` (даже "по границе страницы в 4 кб")
* Для всех разумных типов данных хватит `new char[123]`,
  * Это специфика `new char[]` и `new unsigned char[]`.
  * Выровняет примерно по `max_align_t` (обычно 8 или 16).
* Бывают over-aligned types, но я про них не слышал.

### Вспомогательные функции
* `std::uninitialized_default_construct(T* first, T* last);`
  * Удаляет объекты в __неопределённом__ порядке.
  * Мой `std::vector<>` из MSYS2 удаляет в порядке создания.
* `std::uninitialized_copy(T* first, T* last, T* out)`
  * Тоже вызывает деструкторы при выброшенном исключении.
* И другие

---
## Делегирующие конструкторы и исключения
```c++
struct Foo {
    Foo(int x) {
        some_operation_a(x); // (1)
    }
    Foo() : Foo(10) {
        some_operation_b();  // (2)
    }
    ~Foo() {}
};
```

1. Если вызвали `Foo(10)` и `(1)` выкинуло исключение,
   то деструктор не вызовется.
2. Если вызвали `Foo()` и `(1)` выкинуло исключение,
   то деструктор не вызовется.
3. Если вызвали `Foo(10)` и `(2)` выкинуло исключение,
   то __деструктор вызовется__.

Правило: объект считается живым после успешного завершения
самого вложенного конструктора.

С наследованием надо смотреть на "подобъекты".

---
## Пример inherited constructors
До:
```c++
struct my_exception : std::runtime_error {
    my_exception(const std::string& what_arg) : std::runtime_error(what_arg) {}
    my_exception(const char*        what_arg) : std::runtime_error(what_arg) {}
};
// ...
    throw my_exception("Foo");
// ...
```
После:
```c++
struct my_exception : std::runtime_error {
    using std::runtime_error::runtime_error;
};
// ...
    throw my_exception("Foo");
// ...
```

---
## Пример приватного наследования
```c++
struct noncopyable {  // Или boost::noncopyable
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};
struct my_struct : private noncopyable {
    // ...
};
```

Нельзя будет написать:
```c++
noncopyable *x = new my_struct;
delete x;  // UB, нет virtual ~noncopyable()
```

---
# Инициализация — обзор
Смотри ["Инициализация в современном C++" Тимура Думлера](https://habr.com/ru/company/jugru/blog/469465).

* Напоминание: default initialization, value initialization, direct initialization, copy initialization.
* Агрегатные классы и aggregate initialization
  ```c++
  struct Point { int x, y; };
  // ....
  Point p = {1, 2};
  // ....
  ```
* `std::initializer_list<>`
  ```c++
  vector<Point> p = {{1, 2}, {3, 4}};
  ```
* Напоминание: the most vexing parse, `auto`.
* List initialization (uniform initialization)
* Какой из 19 способов инициализировать `int` выбрать

---
## Старые виды инициализации
* **Default initialization**: `T t;` (в том числе поля), `new T`.
  * Конструктор по умолчанию для классов (возможно, неявный).
  * Ничего не делать для примитивов (мусор, UB).
* **Value initialization**: `new T()`, `T()`, `: member()`.
  * Как и default initialization.
  * Отличие для примитивов: zero initialization (в ноль).
  * Иногда также зануляет для классов, забейте на это.
  * В C++11 добавили: `T t{};`, `new T{}`, `T{}`, `member{}`.

```c++
struct Foo {
    int x;  // default initialization
    int y{};  // value initialization
    int z;
    Foo() : z() /* value initialization */ {}
};
int main() {
    Foo f;  // default initialization
    int z = f.z;  // 0, потому что проинициализирован в member initializer list
    int y = f.y;  // 0
    int x = f.x;  // UB
}
```

---
* **Direct initialization**: `T t(foo, bar)`, `T(foo, bar)`, `static_cast<T>(foo, bar)`, `new T(foo, bar)`, `: member(foo, bar)`.
  * Вызов конструктора `T` с параметрами `foo` и `bar`.
  * В C++11 добавили: `T t{foo, bar};`, `T{foo, bar}`, `new T{foo, bar}`, `member{foo, bar}`.
* **Copy initialization**: `T t = ..;`, `T t = {..}`, `f(t)`, `return ..`, `throw ..`.
  * Вызов _не-explicit_ конструктора `T` с параметрами.
  * В C++11 можно делать несколько параметров.

```c++
struct Point {
  int x, y;
  Point() : x(), y() /* value initialization */ {}
  Point(int x_, int y_) : x(x_), y(y_) /* direct initialization */ {}
};
Point foo(Point p) { return {p.y, p.x} /* copy initialization */; }
int main() {
    foo(Point(10, 20) /* direct initialization */);
    foo({10, 20} /* copy initialization */);
    Point p1{10, 20};  // direct initialization
    Point p2 = {10, 20};  // copy initialization
}
```
---
## Круглые скобки direct initialization
* The most vexing parse:
  ```c++
  struct Timer { .. };
  struct TimeKeeper { TimeKeeper(Timer t) { .. } .. };
  int main() {
      TimeKeeper time_keeper(Timer());
      // Компилируется как объявление глобальной функции:
      // TimeKeeper time_keeper(Timer (*func)());
      // Чинится:
      TimeKeeper time_keeper1((Timer()));  // Лишние скобки.
      auto time_keeper2 = TimeKeeper(Timer());  // Copy initialization вместо direct.
      TimeKeeper time_keeper1{Timer{}};  // Хотя бы один фигурные скобки.
  }
  ```
* Нельзя использовать в полях:
  ```c++
  struct Foo {
      Object1 a(10);  // Не компилируется.
      Object2 b();  // Компилируется как объявление функции
      Object3 c(Object4());  // Компилируется как объявление функции
  };
  ```

---
## Агрегатные типы-1
Их можно инициализировать фигурными скобками, в том числе рекурсивно.

Это **aggregate initialization**: `T t = {..}`, `T t{..}`, `new T{..}`.

* Простые классы. Запрещены:
  * Конструкторы (в некоторых стандартах можно `= delete` и `= default`).
  * `private`/`protected` поля и наследование.
  * Виртуальные функции и виртуальное наследование.
  * Только в C++11: default member initializer: `Foo field(10);`
* Массивы: `Foo a[10];`, может само догадаться про длину.

```c++
struct Point2D { int x, y; };
struct Point3D : Point2D { int z; };
int main() {
    Point2D a{1, 2};
    Point3D b{{1, 2}, 3};  // Записали подряд все базы и члены.
    Point3D arr[] = {{{1, 2}, 3}, {{4, 5}, 6}};
}
```

---
* Для членов/элементов массива происходит copy initialization:
  ```c++
  vector<int> arr1[] = {vector<int>(10) /* direct initialization */};  // Ок
  vector<int> arr2[] = {10};  // Ошибка компиляции, explicit vector<int>(int) 
  ```
* Если какой-то элемент опущен, то для него value initialization (бывает warning для объектов):
  ```c++
  int arr1[5];  // default initialization, в элементах мусор.
  int arr2[5] = {1, 2};  // aggregate initialization, в элементах 1 2 0 0 0
  int arr3[1] = {1, 2};  // Ошибка компиляции
  int arr4[2] = {};  // aggregate initialization, 0 0
  int arr5[4] = {{} /* value initialization */, 1};  // 0 1 0 0
  ```
* Можно опускать почти любые непустые скобочки (brace elision):
  ```c++
  struct Point2D { int x, y; }; struct Point3D : Point2D { int z; };
  int main() {
      Point3D arr[4] = {
          {1, 2, 3},  // {{1, 2}, 3} x=1, y=2, z=3
          {4, 5},     // {{4, 5}, 0} x=4, y=5, z=0
          6, 7, 8     // {{6, 7}, 8} x=6, y=7, z=8
                      // {{0, 0}, 0} x=0, y=0, z=0
      };
  }
  ```

---
* С опущенными скобками весело:
  ```c++
  struct Foo { int x; };
  struct Bar { Foo f; };
  struct Baz { Bar b; };
  int main() {
      Baz b{10};
      assert(b.b.f.x == 10);
  }
  ```
* Если добавить перегрузок, то [очень весело](https://stackoverflow.com/a/54506082/767632):
  ```c++
  f(  {10}  );
  f( {{10}} );
  f({{{10}}});
  ```
* В С++20 добавили designated initializers:
  ```c++
  struct A { int x; int y; int z; };
  A a{.x = 1, .z = 2}; // x=1, y=0, z=2
  ```

Итого: я бы для `Point` не использовал — можно случайно передать одну координату вместо двух.
Но если это окей, можно сделать тип агрегатом.

---
## `std::initializer_list`
```c++
vector<int> f = {1, 2, 3};
// ....
    vector(std::initializer_list<int> l) : storage(l.size()) {
        std::uninitialized_copy(l.begin(), l.end(), storage.begin());
    }
// ....
```
В C++11 можно для своих классов делать что-то похожее на aggregate initialization для массивов:
```c++
struct Foo {
    Foo(std::initializer_list<pair<int, float>> l) {
        // Работаем с l, как с обычным контейнером
    }
};
pair<int, float> p;  // default initialization
Foo f = {            // std::initialize_list
  {1, 2.0f},      // direct initialization
  pair(1, 2.0f),  // direct initialization
  p               // copy initialization
};
```

---
## Тонкости `std::initializer_list`
* Это не агрегатная инициализация: скобки пропускать нельзя:
  ```c++
  vector<pair<int, int>> v1 = {{1, 2}, {3, 4}};  // ОК
  vector<pair<int, int>> v2 = {1, 2, 3, 4};  // Ошибка компиляции
  ```
* Можно делать несколько перегрузок с разными `initializer_list<>`.
* Работает рекурсивно: `vector<vector<int>> = {{1, 2}, {}, {3, 4, 5}};`
* *Минус*: всегда создаётся массив, причём констант:
  ```c++
  vector<string> v{"foo", "bar", "baz"};
  // Перепишется в
  {
    const string temp[3] = {"foo", "bar", "baz"};  // Создаются std::string
    vector<string> v(initializer_list(temp, temp + 3));  // Лишние копии
    // Временные строчки удаляются.
  }
  ```
* Минус: нельзя делать `move` из-за константности, нельзя хранить `unique_ptr`.

---
## Собираем list initialization
Она же "инициализация списком" или "универсальная инициализация" (uniform initialization).

Формальное описание инициализации с фигурными скобками.
Сначала делится на direct и copy аналогично круглым скобкам:

* Direct list initialization:
  * `T object{arg1, arg2, ..};`, в том числе для полей.
  * `T{arg1, arg2, ..}` (в том числе в выражениях, `return`, параметрах)
  * `new T{arg1, arg2, ..}`
  * `: member{arg1, arg2, ..}`
* Copy list initialization:
  * `T object = {arg1, arg2, ..};`
  * `foo({arg1, arg2, ..})` (веселье с перегрузками `f`), в том числе с `operator[]` и `operator=`
  * `return {arg1, arg2, ..};` (нужно уже знать тип возвращаемого значения)

Важно: у выражения со скобками (braced-init-list) **нет типа**!

---
Примерный алгоритм (точный есть на cppreference):
```c++
struct Point { int x, y; };
struct Foo { Foo() { cout << "Foo();\n"; } };
```

* Если `T` — агрерат и у нас ровно один элемент типа `T`, то делаем copy/direct initialization из него.
* Если `T` — массив символов и передали ровно один строковой литерал, то из него.
* Если `T` — агрегат, то aggregate initialization (опускаютя скобки, можно не указывать все члены).
   ```c++
   struct Point { int x, y; };
   Point p1 = {1, 2}; // copy list initialization, aggregate initialization
   Point p2 = p1; // copy initialization
   Point p3 = {p1};  // copy list initialization, copy initialization
   char arr[] = {"ab"};  // copy list initialization, массив символов
   ```
* Если список пуст и есть конструктор по умолчанию, то value initialization.
   ```c++
   Foo f{};  // direct list initialization, value initialization
   ```
---
* Пробуем все конструкторы с `std::initializer_list<>` разных типов.
  Запрещаем сужающие преобразования (`double` в `int`).
* Пробуем все остальные конструкторы.
  ```c++
  vector<int> v1a{3};       // 3
  vector<int> v1b(3);       // direct initialization: 0 0 0
  vector<int> v1c{3.0};     // Ошибка компиляции: сужающее преобразование
  vector<char> v2{3};       // 3
  vector<string> v3{3};     // "", "", ""
  vector<string> v4{3.0};   // Ошибка компиляции: сужающее преобразование
  vector<string> v5 = {3};  // Ошибка компиляции: explicit + copy list initialization
  ```
* Если в списке ровно один элемент, то инициализируем без narrowing conversion.
* Если список пустой, то value initialization.
  ```c++
  int a = 10;    // copy initialization
  int b = 10.0;  // copy initialization
  int x{10};     // direct list initialization
  int x{10.0};   // direct list initialization, ошибка компиляции
  int x{};       // 0, direct list initialization, value initialization
  ```

Ещё есть тонкости про `enum`, ссылки...

---
* С C++11 можно использовать фигурные скобки для инициализации почти чего угодно.
* Иногда поведение отличается от круглых: они более строгие и могут вызывать
  `std::initializer_list` 
  * Круглые в C++20 тоже могут, но с другим приоритетом.
  * Гарантируется порядок вычисления элементов в фигурных скобках: слева направо:
    `Point foo{readInt(), readInt()};`.
  * В круглых не гарантируется, как и в вызовах функций:
* Так как типа у `{}` нет, то весело с перегрузками и надо использовать только там,
  где вы уверены про тип.

Рекомендации по uniform initialization:
```c++
int x = 10;                        // Просто.
vector<int> vec{10, 20};           // Выбора нет: initializer_list
AggregateType x{20, 30};           // Выбора нет: агрегат.
struct Foo { Bar member{20, 30}; } // Выбора нет: default member initialization.
// Для временных значений 
pair<int, string> foo(pair<int, string> x) {
    return {x.first + 10, x.second};
}
foo({20, "xxx"});
// Для вызова конструкторов всё-таки direct.
vector<int> v(4);  // 0 0 0 0
Point p(10, 20);
```

---
<!-- язык: мутабельные лямбды (были на практике) -->
## Как работает вывод типов в шаблонах
Есть лекция Скотта Мейерса ["Type Deduction and Why You Care"](https://www.youtube.com/watch?v=wQxj20X-tIU).

```c++
template<typename T>
void f(PARAM param);
f(EXPR);
```

Надо вывести: что такое `T`, что такое `PARAM`.

Они могут отличаться:

```c++
template<typename T> void printAll(const vector<T>&);
printAll(vector<int>{10, 20});
```

Здесь `PARAM=const vector<int>&`, `T=int`.

---
### `PARAM` — ссылка или указатель
Забили на то, что `EXPR` — ссылка, сделали pattern matching:
```c++
template<typename T> void f(T&);
int x = 10;        f(x);   // T = int, PARAM=int&
const int cx = 20; f(cx);  // T = const int, PARAM=const int&
int &rx = x;       f(rx);  // T = int, PARAM=int&
template<typename T> void g(const T&);
g(x);  // T = int, PARAM=const int&
```
PARAM никогда не ссылка.

#### Вывод типа в `auto`
Считаем, что `auto` — шаблонный параметр:

```c++
int x = 10;
const int cx = 20;
int &rx = x;
auto &arx1 = x;        // auto = int, auto& = int&
*auto &arx2 = cx;       // auto = const int, auto& = const int&
const auto &acx = rx;  // auto = int, const auto& = const int&
```

---
### `PARAM` — не ссылка и не указатель
1. Как и раньше, отбросили ссылку у `EXPR`.
2. Отбросили `const` и `volatile` у `EXPR`.
3. То, что осталось — pattern matching с `PARAM`.

```c++
template<typename T> void f(T);
int x = 10;         f(x);  // T = PARAM = int
*const int cx = 20; f(x);  // T = PARAM = int
int &rx = x;        f(x);  // T = PARAM = int
template<typename T> void g(Foo<T>);
const Foo<const int> v;   g(v);  // T = const int, PARAM = Foo<const Int>
```

При этом `PARAM` никогда не может быть ссылкой или константой.

В `auto` получаем всегда копию:

```c++
vector<int> vec = {1, 2, 3, 4};
const vector<int> cvec = vec;
const vector<int> &rvec = vec;
auto avec = vec;    // auto = vector<int>
auto acvec = cvec;  // auto = vector<int>
auto arvec = rvec;  // auto = vector<int>
```

---
### Вывод возвращаемого типа функции
* Возвращаемое значение лямбды и функции:
  ```c++
  auto lambda = [](int x) { return x; }  // T=int, retval=int
  auto  foo(int x) { return x; }   // auto=int
  auto  foo(int &x) { return x; }  // auto=int
  auto& foo(int &x) { return x; }  // auto=int, auto&=int&
  auto  foo(const int &x) { return x; }  // auto=int, retval=int
  auto& foo(const int &x) { return x; }  // auto=const int, auto&=const int&
  ```
* Функция смотрит на первый `return`, а дальше в точности сойтись:
  ```c++
  auto fac1(int n) { // Окей
      if (n == 1) return 1;
      else        return n * fac1(n - 1);
  }
  auto fac2(int n) { // Ошибка компиляции
      if (n > 1) return n * fac2(n - 1);
      else       return 1;
  }
  auto foo() {
      return 1;
      return 1.0;  // Не сошлось, ошибка компиляции.
  }
  ```

---
### Другие применения вывода типов и `auto`
* В инициализации и возвращаемом типе в лямбдах (но не в захвате):
  ```c++
  [foo = vector<int>(10)]() {}  // auto=vector<int>
  []() -> double { return 10; }
  ```
* `auto` зачем-то может выводить тип в `std::initializer_list` (не надо):
  ```c++
  auto x = {1, 2, 3};
  ```
* Можно писать возвращаемый тип в конце сигнатуры:
  ```c++
  struct Foo {
      using Bar = int;
      Bar foo();
  };
  // Foo::Bar Foo::foo() { .. }
  auto Foo::foo() -> Bar { .. }
  ```
  * Можно смотреть внутрь `Foo::`
  * Можно смотреть на параметры при помощи `decltype()` и других...

---
## `static_cast<Foo>(expr)`
<!--
В C++ преобразования типов разделены на виды.
Для каждого вида есть специальный синтаксис, чтобы при чтении кода
было заметно, где происходит что-то страшное.
-->

* Ваш друг по умолчанию: неявные плюс "точно обратные" к неявным преобразованиям.
* Неявная конвертация к `Foo` (без `explicit`).
* Для ссылок и указателей: basecast (неявно тоже можно), derivedcast (без проверок):
  ```c++
  struct Base1 { int x; };
  struct Base2 { int y; };
  struct Derived : Base1, Base2 {};
  Derived *d = new Derived;
  Base1 *b1 = d;
  Base2 *b2 = d;
  Derived *d1 = static_cast<Derived*>(b1);  // Верно не изменит указатель.
  Derived *d2 = static_cast<Derived*>(b2);  // Верно изменит указатель.
  ```
* Преобразования между числами, в том числе с потерей точности.
* Преобразования от/к `void*`. Гарантируется, что значение указателя такое же.
* `static_cast<void>(foo);` — отключить предупреждение "`foo` не используется".

---
## `reinterpret_cast<Foo>(expr)`

* Делает что-то страшное: просто меняет тип `expr`.
  * Никогда не компилируется в команды процессора.
* Конвертирует указатели в числа и обратно (если места хватает).
* Конвертирует указатели/ссылки в любые другие
  * Значение от промежуточных конвертаций может быть любое, отличается от `static_cast<void*>`.
  * Можно получить UB от strict aliasing rule.
* В нормальном коде на C++ нужен [редко](https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast).

```c++
int *a = new int();
void *b = reinterpret_cast<void*>(a);  // Значение неизвестно!
int *c = reinterpret_cast<int*>(b);  // a == c
float *d = reinterpret_cast<float*>(a);
cout << *d; // UB: нарушение strict aliasing: доступ к
            // объекту типа int через несовместимый float*.
char *e = reinterpret_cast<char*>(a):
cout << *e; // Не UB: любой объект можно читать через char* или 
            // unsigned char* (но не signed char*).
```

---
## `const_cast<Foo>(expr)`
* Делает что-то страшное: отбрасывает константность у `expr`.
* UB, если `expr` на самом деле константный объект.
  ```c++
  vector<int> vec;
  const auto &rvec = vec;
  const vector<int> cvec;
  const_cast<vector<int>&>(rvec).clear();  // Окей
  const_cast<vector<int>&>(cvec).clear();  // UB
  ```
* Можно использовать в своём `operator[]`:
  ```c++
  const T& operator[](size_t i) const { .. }
  T& operator[](size_t i) {
      return const_cast<T&>(static_cast<const vector<T>&>(*this)[i]);
  }
  ```
* Иногда нужен для совместимости с древним Си без `const`:
  ```c++
  void println(char *s);
  println(const_cast<char*>("foo"));
  ```

---
## C-style cast
Синтаксис: `(int)10.5` и похожие `(T)expr`.

* Пробует по очереди:
  * `const_cast`
  * `static_cast` и разрешает ещё пару преобразований
  * `reinterpret_cast`
  * На каждый вариант пытается ещё навесить `const_cast`
* Не надо использовать никогда.
* Практически всегда можно `static_cast` или вызвать конструктор.

---
## `dynamic_cast`
* Для работы с полиформными объектами.
  * Объект полиморофен, если есть хотя бы одна виртуальная функция (обычно хотя бы деструктор).
  * Доступ по указателям или ссылкам (например, `vector<unique_ptr<Figure>>`).
* Позволяет проверить динамический тип объекта во время выполнения.

```c++
struct Base { virtual ~Base(); };
struct Foo : Base {};
struct Bar : Base {};
struct Derived : Foo, Bar {};

Foo *f = ..;
Base *b = f;  // Неявное преобразование (basecast), всегда верно.
Derived *d1 = static_cast<Derived*>(f);  // Derivedcast, иногда UB.
Derived *d2 = dynamic_cast<Derived*>(f);  // Derivedcast, иногда nullptr.
Bar *bar = dynamic_cast<Bar*>(f);  // Crosscast, иногда nullptr, нельзя static.

Foo &rf = f;
Derived &rd1 = static_cast<Derived&>(rf);  // Иногда UB.
Derived &rd2 = dynamic_cast<Derived&>(rf);  // Иногда исключение std::bad_cast.
```
