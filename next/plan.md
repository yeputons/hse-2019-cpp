# Долги с 19.05.2020
* Функтор тоже надо perfect forward! rvalue-ref-qualified, например.

## Parameter pack (variadic template)
Задача: много параметров в perfect forwarding
Синтаксис, группировка, sizeof…
БУДЕТ ПОТОМ: работа с индексами и более сложная распаковка-запаковка, несколько parameter pack

## Что можно получить
* `forward_as_tuple` (и понять, почему типы именно такие), `std::apply`, `std::invoke`
* Можно сделать мок: запоминает все вызовы, потом в тесте проверили.
* Теперь можно сделать объект `log`, который имеет `operator()` и логирует все вызовы и аргументы
  (если они форматируемые), при этом делает perfect forward и сам следит за вложенностью отступов.

# Долги
* User defined literals, не на экзамен: https://en.cppreference.com/w/cpp/language/user_literal
  * Пример из chrono: `auto duration = 10s` (секунд).
  * Пример для автовывода типов: `split(....) == vector{"foo"sv}` (иначе был бы `vector<char*>`).
  * Можно писать свои, можно даже парсить длинные числа руками.
* Был `dynamic_cast`. А для `shared_ptr` есть аналогичный `dynamic_pointer_cast` (и ещё три аналогичных `*_pointer_cast`).
  Это популярно для умных указателей, если вообще имеет смысл менять тип указателя, не меняя тип владения
  (для `unique_ptr` не имеет).
* CRTP:
  * https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Curiously_Recurring_Template_Pattern
  * Можно реализовать для реализации `struct Point : operators<Point> { bool operator<(..); }`
  * Можно для вынесения любой другой функциональности в общего предка без виртуальных функций.
* Pimpl для выноса приватных полей и методов из заголовка и сохранения API/ABI. Ценой динамических выделений памяти.
* declval + comma operator in return value
* tag dispatching вместо SFINAE (можно как специализацию структур, можно by-value параметр функции)
  * Можно по true_type/false_type.
  * В чём бонус по сравнению с обычным SFINAE? Смотри think-cell/range, там это используется.
  * https://stackoverflow.com/questions/6917079/tag-dispatch-versus-static-methods-on-partially-specialised-classes
  * http://barendgehrels.blogspot.com/2010/10/tag-dispatching-by-type-tag-dispatching.html
* Перегрузка `operator->` по цепочке, время жизни возвращённого по значению временного объекта
* Друзья
  * `friend class`
  * Шаблонные друзья
    * https://sysdev.me/druzhestvennyie-shablonnyie-operatoryi/
    * https://blog.panicsoftware.com/friends-and-where-to-find-them/
* Определение метода класса вне класса, который возвращает вложенный класс (`HuffmanTree::Node HuffmanTree::foo()`) — удобно через auto
* `thread_local` переменные
* Ссылки и константы в полях. Внутри контейнеров, пар, кортежей, structured binding, своих шаблонов. Что, когда, как.
  * Const tuple<int&> хранит внутри себя ссылку на неконстантный объект. Плохо играет со structured binding. Примерно как `const tuple<int*>`: https://stackoverflow.com/a/49309088/767632
* Распад аргументов (надо при сохранении в поля)
  * `std::array` (не decay’ится)
  * `decay_t` вместо `remove_cvref`

## Метапрограммирование
1. clang + enable_if = хорошие сообщения об ошибках. проверить, показать
2. если мы делаем свой sfinae detector, то осторожно с ADL при его вызове.
3. шаблонные параметры конструктора не указать явно?
4. Нельзя шаблонизировать конструкторы/операторы копирования/присваивания: компилятор сгенерирует версию по умолчанию,
   которая будет приоритетнее `template<typename T> MyClass(const T&)`.
   * Swap trick работает, потому что стандарт разрешает `operator=(MyClass)`.
   * Видимо, надо ооочень аккуратно смотреть на user-defined/user-declared/implicitly defined-deleted, whatever.

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

# Ещё дальше
* Кого читать про C++:
  * ISO C++ FAQ
  * CoreCppGuidelines
  * GotW (Herb Sutter)
  * Andrei Alexandrescu
  * Scott Meyers
  * TotW (Google)
