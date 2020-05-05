# Разбор теста
https://stackoverflow.com/questions/4316727/returning-unique-ptr-from-functions/4316948#4316948

# Свойства выражений
## Самостоятельное определение свойств типов через SFINAE
http://jguegant.github.io/blogs/tech/sfinae-introduction.html
### До C++11 без `declval`/`decltype`: `sizeof`
### Detection idiom, `decltype`
### для игнорирования ошибок в объявлениях (не в определениях)
### Показать, что на SFINAE можно наткнуться и просто так
https://godbolt.org/z/qkjL7C

## C++20, анонс: концепты с requires expression (typename, проверка типов)

# Проверка "что нам подсунули"
## Включение-выключение
### Типов (специализация по условию)
#### `enable_if_t`
### Полей и родителей через наследование
* Например, когда у нас тип — `void`. Можно ещё свою обёртку сделать, которая занимает один байт, и её специализировать.
### Перегрузок
* И зачем это всё? А чтобы работали всякие `is_move_constructible` — они смотрят только на объявление, не определение.
#### `enable_if_t` в возвращаемом значении, `auto ->`
#### `typename = enable_if_t` и `enable_if_t<...>* = nullptr`
https://stackoverflow.com/a/31138725/767632

## `static_assert`

## C++20, анонс:
https://habr.com/ru/company/jugru/blog/467299/
### Terse syntax для ограничений над auto (переменные, параметры, возвращаемые типы)
### Requires clause

# Адаптируемся под то, что подсунули
## `if constexpr`
* Раньше делали через специализацию структур и вспомогательную функцию
### `static_assert(false)`
### `resize()` вектора без буфера

## Включение-выключение для выбора поведения

## Свой `std::conditional` и выбор разных типов.
### `move_if_noexcept`
### Если дали геттер — верни `const&`, ещё и сеттер — `&`

## tag dispatching (уже был)

# Для лабы
## Синтаксис `<auto... Params>` и `<auto Param>` с C++17
