list-initialization (uniform initialization syntax).

# Детали наследования и классов
## Добавление перегрузок к методам
```
struct Foo {
    void foo(int);
};
struct Bar : Foo {
    // using Foo::foo;
    void foo(double);
};
Foo f;
f.foo(1.2); // int
f.foo(1); // int
Bar b;
b.foo(1.2); // double
b.foo(1); // double, после using - int.
```

## Множественное наследование
## Виртуальное наследование

# Прочие долги
## Статические члены классов-константы и строчки
* Напоминание: статические члены классов-переменные: обычно должно объявляться в классе
  и определяться снаружи (причём ровно один раз):
  ```
  // h
  extern int x;
  struct A {
      static int x;
  };
  // cpp
  int x = 10;
  int A::x = 10;
  ```
* Константы аналогично:
  ```
  // h
  extern const int y;
  extern const char * const s1;
  struct A {
      static const int y;
      static const char * const s1;
  };
  // cpp
  const int y = 10;
  const char* const s1 = "foo";
  const int A::y = 10;
  const char* const A::s1 = "foo";
  ```
* Но это не всегда удобно.
  * Надо писать константу не там, где объявлена, даже если это простое число.
  * Компилятору сложнее оптимизировать константы.
  * Нельзя использовать этот `const` для размеров массивов (GCC сможет, потому что
    он умеет в variable length array, VLA).

TODO

* Как объявлять константы (числа, строчки) как статические члены классов.
9.4.2/4
> If a static data member is of const integral or const enumeration type,
> its declaration in the class definition can specify a constant-initializer
> which shall be an integral constant expression (5.19). In that case, the
> member can appear in integral constant expressions. The member shall still
> be defined in a namespace scope if it is used in the program and the namespace scope definition shall not contain an initializer.

Только integral type (), enum, char

* `static inline` vs `static constexpr` (лучше второе, чтобы была гарантия)

## ADL
https://en.cppreference.com/w/cpp/language/adl
Для `operator<<`
Для `swap`: https://stackoverflow.com/a/5695855
`namespace internal` для отключения случайного ADL

# Ещё дальше
* C++ позволяет специализировать и инстанциировать шаблоны от приватных типов, а оттуда `friend`'ом вытащить, тогда не будет ошибки "тип приватный" (только практики?): https://godbolt.org/z/hH9H27
* Stateful metaprogramming (только практики?): https://stackoverflow.com/questions/44267673/is-stateful-metaprogramming-ill-formed-yet
