# Долги [00:15]
* `make_shared` ещё и меньше выделений памяти делает и работает быстрее.
* `span`/`string_view` используем вместо итераторов
* Про вектор один слайд:
  1. Можно использовать `new char[]` вместо `aligned_alloc` и оно будет адекватно работать для почти всех типов: https://stackoverflow.com/a/10588226
  1. Есть `uninitialized_copy`, `uninitialized_default_construct` и прочие.
* Делегирующие конструкторы: объект считается живым после самого вложенного конструктора.
* Пример для inheriting constructors: наследование от `runtime_error`, тогда не надо писать свой.
* Приватное наследование нужно для, например, `boost::noncopyable`.

# Инициализация [00:15]
* `initializer_list`
  * braced-init-list не имеет типа (в отличие от всех остальных), просто пихаем в `std::initialize_list` (внезапно даже в `auto`).
  * Вложенность, elision
* Uniform initialization syntax
* Aggregate initialization
* Как выбирается, чем инициализировать
* Чем рекомендуется инициализировать. The most vexing parse.

# Язык [00:20]
* `auto`: пять применений
  * https://www.youtube.com/watch?v=wQxj20X-tIU
  * Автовывод в шаблонах: `T`, `T&/T*`, как влияет `const`/`volatile`.
  * Автовывод возвращаемого значения в обычной функции (взяли первую ветку, дальше сошлось), как в шаблонах без тонкостей.
  * Синтаксис `auto foo() -> int`
  * Автовывод возвращаемого значения лямбды как в `T`.
  * Автовывод в `auto` переменных и lambda init capture как в шаблонах с тонкостью (не надо писать braced-init-list, он разный в C++14, C++17, см. лекцию).
  * ЗАБИТЬ: Автовывод в lambda capture: как `T&/T*` с тонкостью.

# Касты [00:15]
* `static_cast`
* `const_cast`
* `reinterpret_cast`
  * strict aliasing rule
* `dynamic_cast`
  * По ссылке и по указателю, исключения.
  * Cross-cast
* C-style cast, когда какой каст использовать
