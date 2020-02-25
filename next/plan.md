list-initialization (uniform initialization syntax).

# Долги
Приватное наследование нужно для, например, boost::noncopyable.
Или какого-нибудь другого свойства, которое требуется всем экземплярам класса.

# Move-семантика — реализация [00:30]
## Категории значений [00:10]
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
    * `this`
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

## rvalue-ссылки [00:05]
* C++11 добавляет новый вид ссылки: rvalue-ссылка: `Foo&& x = foo();`
* Это работает точно так же, как обычная константная ссылка: может биндится только к rvalue (xvalue, prvalue).
* А обычная ссылка может биндится только к glvalue. К prvalue не может.
* `const Foo&&` — хрень, не пишите так. Забиндится, но move не сработает.

## Move-семантика — конструктор перемещения и оператор перемещающего присваивания [00:10]
* Конструктор перемещения — просто перегрузка:
  ```
  stack(const stack &other) : data(new ...), ... {}
  stack(/*const, WTF */ stack &&other) /*noexcept*/ : data(std::move(other.data)), cap(std::move(other.cap)), len(std::move(other.len)) {} {
      // Здесь важно, что std::move(other.data), потому что это unique_ptr.
      // А дальше важно занулить cap/len. std::move — это просто смена категории. Для примитивных типов ничем от копирования не отличается.
      other.cap = other.len = 0;
  }
  ```
* Аналогично с оператором присваивания:
  ```
  stack& operator=(stack &&other) /* noexcept */ {
      if (this == &other) return *this;
      data = std::move(other.data);
      cap = other.cap;
      len = other.len;
      other.cap = other.len = 0;
      return *this;
  }
  ```
  Или можно просто `swap`. Нам же необязательно оставлять `other` пустым.
* И тут copy-and-swap позволяет нам не реализовывать два оператора:
  ```
  stack& operator=(stack other) {
      swap(data, other.data);
      return *this;
  }
  ```

## Move-семантика в стеке [00:05]
* Осторожно с universal reference (с ними будем потом):
  * `template<typename T> foo(T&& x) {}` // move не ок; может схватить и lvalue.
  * `template<typename T> foo(vector<T>&& x) {}` // move ок, схватит только rvalue.

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

# Статические члены классов-константы [00:15]
## Линковка обычных констант [00:05]
* Обычно слово `const` для глобальных переменныех автоматически влечёт
  за собой internal linkage: видно только из текущей единицы трансляции.
* Поэтому можно объявлять константы в заголовке и всё работает без ошибок
  multiple definition.
  ```
  // a.h
  const int x = 10;
  // a.cpp
  #include "a.h"
  // b.cpp
  #include "a.h"  // Забыли перекомпилировать после изменения a.h, упс.
  ```
* Заумность: если не хотим — можно дописать `extern`, будет `extern const int x/* = 10*/;`.
  Но так не надо.

## Старый способ для статических [00:07]
* Напоминание: статические члены классов-переменные: обычно должно объявляться в классе
  и определяться снаружи (причём ровно один раз):
  ```
  // h
  struct A {
      static int x;
  };
  // cpp
  int A::x = 10;
  ```
* Константы аналогично:
  ```
  // h
  struct A {
      /*static*/ const int y;
      static const char * /*const*/ s1;
  };
  // cpp
  const int A::y = 10;
  const char* const A::s1 = "foo";
  ```
* Но это не всегда удобно.
  * Надо писать константу не там, где объявлена, даже если это просто число.
  * Компилятору сложнее оптимизировать константы в других единицах трансляции.
  * Нельзя использовать этот `const` для размеров массивов (GCC сможет, потому что
    он умеет в variable length array, VLA):
    ```
    int arr[A::y];
    ```
* Если у нас статическая константа целочисленного типа или enum,
  то можно проинициализировать константной прямо в объявлении.
  ```
  // h
  struct A {
      static const int y = 10;
  };
  ```
  Тогда можно использовать в константных выражаниях.
  * Но если ODR-used, то всё равно нужно ровно одно определение.
  * Пример, когда не нужно: заиспользовали как размер массива: `int arr[A::y];`
  * Пример, когда нужно: взяли адрес: `foo(&A::y);`
  * Пример, когда нужно: инициализируется сложной функцией: `static const int y = foo();`
* Заумность: с C++17 можно сделать inline-переменную (отключит ODR),
  получим `static inline const`, но лучше не надо.

## Хороший способ [00:03]
* Любую константу пробуете сделать `constexpr` вместо `const`.
  Это не часть типа (но автоматически влечёт `const`),
  это дополнительное ограничения компилятору: вычисли на этапе компиляции.
* В противном случае честно объявляете в `.h` и определяете в `.cpp`.

## ADL
Вызов конструктора не находится через ADL (и слава богу, теперь namespace no_adl работает).
friend namespace injection в стандартном C++ нет
https://en.cppreference.com/w/cpp/language/adl
Для `operator<<`
Для `swap`: https://stackoverflow.com/a/5695855
`namespace internal` для отключения случайного ADL
# Ещё дальше
* C++ позволяет специализировать и инстанциировать шаблоны от приватных типов, а оттуда `friend`'ом вытащить, тогда не будет ошибки "тип приватный" (только практики?): https://godbolt.org/z/hH9H27
* Stateful metaprogramming (только практики?): https://stackoverflow.com/questions/44267673/is-stateful-metaprogramming-ill-formed-yet
