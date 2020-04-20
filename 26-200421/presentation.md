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
  * Напишете на практике свой `std::function`/`std::any`.
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
## 1.2.1 Передача параметров в функции и умные указатели
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

* Если хотим отдать параметр/переменную — всегда пишем `std::move`:
    ```c++
    struct MyVector {
        vector<int> data;
        MyVector(vector<int> data_) : data(std::move(data_)) {
            // Тут состояние data_ не определено.
        }
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
## 1.2.3. Умные указатели в параметрах
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
Node(const Node &left_, const Node &_right)
    : left(make_unique<Node>(left_)), .... {}
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

Обязаны определить, причём ровно один раз (ODR).
Не всегда, но лучше не рисковать (IFNDR хуже UB).

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
constexpr char Foo::Name[];
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

Слово `static` в определении не нужно.
Всё ещё неидеальное решение.

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
