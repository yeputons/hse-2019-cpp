## `make_shared`
Если пишем `std::shared_ptr ptr(new T());`, то в памяти:
```
┌───┐  ┌───────────────┐
│ T │  │shared_ptr_data│
└───┘  └───────────────┘
```
Если пишем `std::make_shared<T>()`, то выделяем один блок рядом:
```
┌───┬───────────────┐
│ T │shared_ptr_data│
└───┴───────────────┘
```
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

Можно даже сделать без лишних копирований:
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

С наследованием надо смотреть независимо по всем базам.

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
noncopyable* x = new my_struct;
delete x;  // UB, нет virtual ~noncopyable()
```
