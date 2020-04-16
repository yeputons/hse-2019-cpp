# Короткие обзоры [00:30]
* `if (auto it = m.find(key); it != m.end()) cout << it->second;`, также работает в ветке else(?)
  Используйте активно
* Был `dynamic_cast`. А для `shared_ptr` есть аналогичный `dynamic_pointer_cast` (и ещё три аналогичных `*_pointer_cast`).
  Это популярно для умных указателей, если вообще имеет смысл менять тип указателя, не меняя тип владения
  (для `unique_ptr` не имеет).
* CRTP для реализации `struct Point : operators<Point> { bool operator<(..); }`
* Pimpl для изоляции API/ABI ценой динамических выделений памяти (TODO).
  * Долго осознавать
* User defined literal: пример из chrono, пример для `split() == vector{"foo"sv}`, можно писать свои двух видов (TODO)
* `std::tuple` , `tuple_size`, `get`, `make_tuple`, нет циклов
* Structured binding с `auto`/`auto&`/`const auto&`
  * Детали!
* Как правильно объявлять обычные константы и статические константы в заголовках для чисел, массивов символов, объектов.

# Namespaces и ADL [00:50]
https://en.cppreference.com/w/cpp/language/adl
TODO

* ADL для поиска операторов и функций
  * Идея-1: формально находить `operator<<`
  * Идея-2: свободная функция в целом — кусок интерфейса класса
  * http://www.gotw.ca/gotw/030.htm
  * http://www.gotw.ca/publications/mill02.htm
  * ?? Посмотреть C++ Core Guidelines, C++ ISO FAQ, Мейерса, Александреску, ...
* Как писать `std::swap` и почему: https://stackoverflow.com/a/5695855
* Зачем нужен ADL, кроме операторов и костыльного swap (разрешить бы частичные специализации в `std::` и всё)
  * ADL-only
    * range-based-for поиск std::begin/std::end
    * Structured binding поиск get<>
  * Поиск друзей: friend namespace injection в стандартном C++ нет, как добавить? 
* `namespace no_adl`+`using` для отключения случайного ADL?
  * Вызов конструктора не находится через ADL (и слава богу, теперь namespace no_adl работает). TODO
