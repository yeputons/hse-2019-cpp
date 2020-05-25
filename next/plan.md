# Parameter pack (variadic template)
Задача: много параметров в perfect forwarding

* Синтаксис, группировка, sizeof…

Работа с индексами и более сложная распаковка-запаковка, несколько parameter pack.
Задача: strcat, который выводит в stringstream. Может с разделителем.

* Захват parameter pack в лямбду
* Index_sequence, make_index_sequence
* Шаблонные лямбды (`auto` и `[]<template>()`), чтобы не делать invokeImpl, а делать IIFE
* fold expressions с C++17: делаем strcat наивно с operator<<
* Рекурсия в функциях для сложных вычислений
* `initializer_list` в конструкторах

Реализация tuple через рекурсивное наследование.

* Синтаксис `<auto ...Params>` и `<auto Param>` с C++17.

## Что можно получить
* `forward_as_tuple` (и понять, почему типы именно такие), `std::apply`, `std::invoke`
* Теперь можно сделать объект `log`, который имеет `operator()` и логирует все вызовы и аргументы
  (если они форматируемые), при этом делает perfect forward и сам следит за вложенностью отступов.
* Можно сделать мок: запоминает все вызовы, потом в тесте проверили.
* Распад аргументов (надо при сохранении в поля?)
  * `std::array` (не decay’ится)
  * `decay_t` вместо `remove_cvref`

# Важные долги для полной картины
* `friend class`
* `thread_local` переменные
* Определение метода класса вне класса, который возвращает вложенный класс (`HuffmanTree::Node HuffmanTree::foo()`) — удобно через auto
* Перегрузка `operator->` по цепочке, время жизни возвращённого по значению временного объекта
* Нельзя шаблонизировать конструкторы/операторы копирования/присваивания: компилятор сгенерирует версию по умолчанию,
  которая будет приоритетнее `template<typename T> MyClass(const T&)`.
  * Swap trick работает, потому что стандарт разрешает `operator=(MyClass)`.
  * Видимо, надо ооочень аккуратно смотреть на user-defined/user-declared/implicitly defined-deleted, whatever.
* Функтор тоже надо perfect forward! rvalue-ref-qualified, например.

# Обзор
## Метапрограммирование
* declval + comma operator in return value
* tag dispatching вместо SFINAE (можно как специализацию структур, можно by-value параметр функции)
  * Можно по true_type/false_type.
  * В чём бонус по сравнению с обычным SFINAE? Смотри think-cell/range, там это используется.
  * https://stackoverflow.com/questions/6917079/tag-dispatch-versus-static-methods-on-partially-specialised-classes
  * http://barendgehrels.blogspot.com/2010/10/tag-dispatching-by-type-tag-dispatching.html
* type tags (emplace, конструкторы any):
  * `nullopt`
  * `in_place`, `in_place_type` позволяют отличить конструктор копирования от чего-то ещё
  * Парсер-комбинаторы: `many(many(...))` будет копировать, если `many` — класс. А вот если функция, то всё честно.

## Не про метапрограммирование
* Precompiled header для ускорения компиляции
* User defined literals, не на экзамен: https://en.cppreference.com/w/cpp/language/user_literal
  * Пример из chrono: `auto duration = 10s` (секунд).
  * Пример для автовывода типов: `split(....) == vector{"foo"sv}` (иначе был бы `vector<char*>`).
  * Можно писать свои, можно даже парсить длинные числа руками.
* Был `dynamic_cast`. А для `shared_ptr` есть аналогичный `dynamic_pointer_cast` (и ещё три аналогичных `*_pointer_cast`).
  Это популярно для умных указателей, если вообще имеет смысл менять тип указателя, не меняя тип владения
  (для `unique_ptr` не имеет).

## Идиомы
https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms

* CRTP:
  * https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Curiously_Recurring_Template_Pattern
  * Можно реализовать для реализации `struct Point : operators<Point> { bool operator<(..); }`
  * Можно для вынесения любой другой функциональности в общего предка без виртуальных функций.
* Pimpl для выноса приватных полей и методов из заголовка и сохранения API/ABI. Ценой динамических выделений памяти.
* Return type resolver (+Builder)
* Expression templates (например, для linq).

## Техника метапрограммирования: грабли
* Шаблонные друзья
  * https://sysdev.me/druzhestvennyie-shablonnyie-operatoryi/
  * https://blog.panicsoftware.com/friends-and-where-to-find-them/
* Ссылки и константы в полях. Внутри контейнеров, пар, кортежей, structured binding, своих шаблонов. Что, когда, как.
  * Const tuple<int&> хранит внутри себя ссылку на неконстантный объект. Плохо играет со structured binding. Примерно как `const tuple<int*>`: https://stackoverflow.com/a/49309088/767632
* Если мы делаем свой sfinae detector, то осторожно с ADL при его вызове.
* Шаблонные параметры конструктора не указать явно?
