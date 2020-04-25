# Мотивация [00:05]
## Хотим обобщить
* `std::distance` должен работать для разных итераторов по-разному (уже известно)
* `vector<>` должен делать `move` или `copy` элементов в зависимости от noexcept
* `std::any`/`std::function` — хотим small object optimization. Надо менять поведение в зависимости от того, что скормили в конструктор.

## Хотим, чтобы компилятор делал рутину за нас
* Запустить много алгоритмов на одних и тех же данных и проверить, что будет.
* Вывести на экран сложный класс с векторами и мэпами, но описать это просто: TODO
* "Регистрируем" функции в интерпретаторе арифметических выражений
* Dependency injection

# Базовые конвенции [00:15]
## Вычисления без constexpr [00:01]
* Можно использовать любые конструкции языка.
* Можно объявлять свои классы, поля, переменные.
* Можно менять переменные.
* Можно выделять память.

## Вычисления с constexpr [00:02]
* Можно использовать многие конструкции языка: нельзя `goto` и не-`constexpr` функции.
* Можно использовать скалярные типы и типы с `constexpr`-конструктором и тривиальным деструктором (вроде `optional` от простых типов).
* Всё ещё можно менять переменные внутри функций!
* До C++20 нельзя выделять память => нельзя `vector`
```c++
constexpr int factorial(int n) {
    int res = 1;
    for (int i = 1; i <= n; i++)
        res *= i;
    return res;
}
static_assert(factorial(5) == 120);
```

## Строковые литералы [00:05]
* Нет `constexpr strlen`, но всё ещё можно делать цикл до `s[i]`.
* Лучше не `const char *s`, а `const char (&s)[N]` с шаблонным `N`
* Можно парсить в compile-time:
```c++
template<size_t N>
constexpr auto count_specifiers(const char (&s)[N]) {
    int specifiers = 0;
    array<char, N> found{};
    for (size_t i = 0; i < N; i++) {
        if (s[i] == '%') {
            if (i + 1 >= N)
                throw std::logic_error("Expected specifier after %");
            i++;
            found[specifiers++] = s[i];
            if (!(s[i] == 'd' || s[i] == 'c' || s[i] == 's'))
                throw std::logic_error("Unknown specifier");
        }
    }
    return pair{specifiers, found};
}
static_assert(count_specifiers("hello%d=%s").first == 2);
static_assert(count_specifiers("hello%d=%s").second[0] == 'd');
static_assert(count_specifiers("hello%d=%s").second[1] == 's');
```
* Так можно распарсить что угодно и получить информацию для дальнейшего исследования.

## Виды вычислений во время компилции [00:07]
* Ещё с C++98 бывают функции из типов в типы:
  ```c++
  template<typename T>
  struct iterator_traits {  // Общий случай
      using value_type = typename T::value_type;
      using difference_type = typename T::difference_type;
  };
  template<typename T>
  struct iterator_traits<T*> {  // Частный случай
      using value_type = T;
      using difference_type = std::size_t;
  };
  ```
* Если такая "функция" возвращает ровно один тип, то обычно его называют `type`
  ```c++
  // remove_const
  ```
  * Как использовать: `typename ...::type`.
  * С C++11 есть конвенция создавать template using с суффиксом `_t`, тогда typename не нужен.
    Получаем вызов функции, только `<>` вместо `()`, и в качестве параметров — и значения, и типы.
* Ещё с C++98 иногда нужно вернуть ровно одно число.
  * Пример реализации `std::rank<int[][]>::value == 2`
    * Ещё давным-давно вместо `static constexpr` писали `enum { value = 10 }`, потому что бажные компиляторы: https://stackoverflow.com/a/205000/767632
  * Конкретно тут не хватит `constexpr`-функции, потому что потребуются частичные реализации функций + перегрузки.
  * Как использовать: `::value`
  * С C++17 есть конвенция создавать template variable с суффиксом `_v`.
    Получаем вызов функции, только `<>` вместо `()`, и в качестве параметров — и значения, и типы.
* С числами в параметрах бывают ограничения:
  ```c++
  template<typename T, T N> struct fac { .... };
  template<typename T>      struct fac<T, 0> { .... };  // Упс :(
  ```
  * Их можно обходить как раз при помощи дополнительного уровня `integral_constant`:
    ```c++
    template<typename N> struct fac {};  // Базовый случай, не вызывается.
    template<typename T, T N> constexpr auto fac_v = fac<std::integral_constant<T, N>>::value;
    template<typename T, T N> struct fac<std::integral_constant<T, N>> : std::integral_constant<T, N * fac_v<T, N - 1>> {};
    template<typename T>      struct fac<std::integral_constant<T, 0>> : std::integral_constant<T, 1> {};
    static_assert(fac_v<int, 5> == 120);
    ```

# Type traits [00:20]
* Нестрогое определение: такие классы/структуры, которые только
  таскают информацию в типах,  называются "типажи" (traits).
* Например, есть встроенные в язык "кирпичики" из `type_traits`:
  * Реализация `is_same`
  * `is_convertible` (без реализации)
  * `is_constructible` (без реализации)
  * Реализация `is_integral`
  * `is_polymorphic` (нельзя реализовать)
  * `remove_cv`
  * Некоторые можно написать самому или взять `boost::function_traits`
    ```c++
    template<typename T>
    struct function_traits {};
    template<typename Ret, typename Arg0>  // Можно написать обобщённо для N аргументов (variadic template), пока не будем.
    struct function_traits<Ret(Arg0)> {
        static constexpr std::size_t arity = 1;
        using return_value = Ret;
        using arg_0 = Arg0;
    };
    ```
* Некоторые traits предполагаются для расширения пользователем.
  * `iterator_traits` — по умолчанию берёт `typename` изнутри типа, но мы вольны
    дать свою интерпретацию любому типу вообще.
  * `char_traits` — по умолчанию даёт `char_type`/`int_type`, `eof()`/`eq`
    для `char`/`wchar_t` и ещё кого-то, но мы можем добавлять свои.
    * Например, сделать обёртку `case_insensitive_char` и для неё специализировать.
    * Или вообще в `basic_string` напрямую передать
* Так можно любые типы адаптировать под любую библиотеку.
  Например, сделать свою библиотеку (де)сериализации и сделать `serialization_trait`
  с двумя статическими функциями.
  * А потом пользователи могут его для своих типов использовать.
  * А мы можем сказать, что реализация по умолчанию смотрит на какие-то методы.

# Указатели на члены класса и функции [00:10]
(методы/поля), на функции (в том числе выбор перегрузки)
Std::addressof
.* ->* и скобочки вокруг (почему-то в умных указателях ->* нет)

Только `&C::m`, `&(C::m)` нельзя
Есть касты к дочерним членам (потому что там функция точно есть). В обратную сторону тоже можно, но явно.

Выбор перегрузки происходит в момент присваивания в указатель (в точности должно сойтись).
Если присваиваем в `auto`/шаблонный параметр — не катит.

# Свойства выражений [00:50]
* Про типы всё можем узнавать, теперь давайте что-нибудь узнаем про выражения и функции.

## Оператор `noexcept(....)` и `declval<>` [00:05]
* Возвращает `true`/`false` на этапе компиляции: верно ли, что выражение внутри `noexcept`.
* Пример: `int a = 10; static_assert(noexcept(a = 20));`.
* Внутри `noexcept` иногда нужно создать значение определённого типа, для
  этого есть `std::declval<int>()` (также есть `<int&>`; `<int&&>` эквивалентно `<int>`).
  * Это легально только если это значение никогда не может быть вычислено.

## Применение `noexcept` [00:10]
Полезно условного `vector`:

* Если у элементов `is_nothrow_move_assignable`, то можно делать `operator=(vector&&)` без промежуточного буфера и получить строгую гарантию исключений.
* Если у элементов `is_nothrow_move_assignable`, то можно в `reserve` делать сразу `move` вместо копирований.
  Для этого даже есть сахар: `move_if_noexcept`, реализация.
* А можно делать у `optional::operator=()` условный `noexcept`, получим `noexcept(noexcept(....))`

## SFINAE [00:05]
Цель: научиться определять, есть ли у типа определённый метод вроде `iterator begin()`.

http://jguegant.github.io/blogs/tech/sfinae-introduction.html

Базовый блок: Substitution Failure Is Not An Error (SFINAE).

* Если в _объявлении_ функции случилась ошибка при подстановке шаблонных типов, забиваем на эту перегрузку.
* Пример: `template<typename T> typename iterator_traits<T>::value_type dereference(T it) { return *it; }`
* Пример с деталями:
  * В процессе подстановки default template parameter тоже окей
  * Default values уже не часть объявления для целей SFINAE, будет hard ошибка.
  * ЗАБИТЬ: почему `typename = enable_if` не работает, а `enable_if_t<>* = nullptr` работает: https://stackoverflow.com/a/31138725/767632

## Member detector idiom, C++98 style [00:10]
TODO

Добавляем две перегрузки функции `detector`: одна возвращает `yes`, другая `no`, причём первая вырезается по SFINAE

* Вторая принимает ellipsis и выберется последней. Ей тоже нужны шаблонные параметры.
* У первой функции надо подобрать такие параметры и шаблонные типы, чтобы выводились только при наличии `begin()`.
  * Можно сказать, что это будет какое-то количество указателей неважно куда и мы вызовем с `nullptr`.
  * Тогда результат выражения — либо `yes`, либо `no`, и по `sizeof()` мы можем определить, что именно.

* Попросим в параметре указатель на член определённого типа и скажем, что он равен `&C::begin`
  * Увы, в параметрах по умолчанию SFINAE не работает.
* Костылим вроде `integer_sequence`: делаем свой `template<typename T, T> struct Check{}`
  и просим в качестве параметра указатель на `Check<typename C::iterator (C::*)(), &C::begin>`.

* Теперь по `sizeof(detector<T>(nullptr))` можем определить, 

##
declval + comma operator in return value
