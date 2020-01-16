list-initialization (uniform initialization syntax).

# Ещё дальше
* Ошибки в описании своих манипуляторов: на самом деле `operator()` надо потому руками перегружать с `operator<<`.
  Там только для свободных функций есть `operator<<`.
* Как объявлять константы (числа, строчки) как статические члены классов.
* ADL:
  https://en.cppreference.com/w/cpp/language/adl
  Для `operator<<`
  Для `swap`: https://stackoverflow.com/a/5695855
  `namespace internal` для отключения случайного ADL
* C++ позволяет специализировать и инстанциировать шаблоны от приватных типов, а оттуда `friend`'ом вытащить, тогда не будет ошибки "тип приватный" (только практики?): https://godbolt.org/z/hH9H27
* Stateful metaprogramming (только практики?): https://stackoverflow.com/questions/44267673/is-stateful-metaprogramming-ill-formed-yet
