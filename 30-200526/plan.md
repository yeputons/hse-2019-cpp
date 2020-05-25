# Разбор теста [00:20]
TODO

# Долги [00:20]
TODO

## Перегрузка `operator->` [00:05]
* Перегрузка `operator->` по цепочке
  * Пример: возвращаем умный указатель
* Время жизни возвращённого по значению временного объекта (как обычно, до конца full expression)

## Rvalue-ref-qualified functors [00:05]
* Функтор тоже надо perfect forward! rvalue-ref-qualified, например.

## Мелочи [00:10]
* Precompiled header для ускорения компиляции
* `friend class`
* `thread_local` переменные (работают как `static`)
* Определение метода класса вне класса, который возвращает вложенный класс (`HuffmanTree::Node HuffmanTree::foo()`) — удобно через auto
* Нельзя шаблонизировать конструкторы/операторы копирования/присваивания: компилятор сгенерирует версию по умолчанию,
  которая будет приоритетнее `template<typename T> MyClass(const T&)`.
  * Swap trick работает, потому что стандарт разрешает `operator=(MyClass)`.
  * Видимо, надо ооочень аккуратно смотреть на user-defined/user-declared/implicitly defined-deleted, whatever. TODO
* declval + comma operator in return value

# Parameter pack (variadic template) [00:40]
TODO

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
