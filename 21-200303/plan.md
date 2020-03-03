# Move-семантика — реализация [00:30]
## Категории значений [00:20]
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
    * `this` (именно сам указатель)
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

## rvalue-ссылки [00:10]
* C++11 добавляет новый вид ссылки: rvalue-ссылка: `Foo&& x = foo();`
* Это работает точно так же, как обычная константная ссылка: может биндится только к rvalue (xvalue, prvalue).
* А обычная ссылка может биндится только к glvalue. К prvalue не может.
* `const Foo&&` — хрень, не пишите так. Забиндится, но move не сработает.
* Конструктор перемещения — просто перегрузка:
  ```
  stack(const stack &other) : data(new ...), ... {}
  stack(/*const, WTF */ stack &&other) /*noexcept*/ : data(std::move(other.data)), cap(std::move(other.cap)), len(std::move(other.len)) {} {
      // А дальше важно занулить cap/len. std::move — это просто смена категории. Для примитивных типов ничем от копирования не отличается.
      other.cap = other.len = 0;
  }
  ```

## Опционально: perfect forwarding и forwarding-ссылки [00:20]

# Специализации шаблонов [00:25]
## Синтаксис для `stack<bool>` [00:05]
```
template<typename T> struct stack { ... }; // Общий случай
template<>
struct stack<bool> { // Для конкретных параметров.
    // Абсолютно независимая реализация.
};
```

## Частичная специализация [00:05]
```
template<typename T> struct unique_ptr {
    T *data;
    ~unique_ptr() { delete data; }
};
template<typename T> struct unique_ptr<T[]> {
    T *data;
    ~unique_ptr() { delete[] data; }
};
```

Работает pattern matching.
Можно зачем-нибудь специализировать как `template<typename A, typename B> struct unique_ptr<map<A, B>> { ... }`.

## Немного type traits [00:15]
Так можно делать вычисления над типами и значениями на этапе компиляции:
```
template<typename T> struct is_reference { constexpr static bool value = false; };
template<typename T> struct is_reference<T&> { constexpr static bool value = true; };
template<typename T> struct is_reference<T&&> { constexpr static bool value = true; };
//
printf("%d\n", is_reference<int&>::value);
```

Это один из способов реализовывать `<type_traits>` (второй будет в 4 модуле, называется SFINAE).

Ещё пример:
```
template<typename U, typename V> struct is_same { constexpr static bool value = false; };
template<typename T> struct is_same<T, T> { constexpr static bool value = true; };
```

А ещё это можно использовать в своих библиотеках.
Например, для сериализации: вы пишете общий случай `template<typename T> struct serializer {};`,
а дальше для каждого типа реализуете `struct serializer { static std::string serialize(const T&); static T deserialize(std::string); }`

## Специализация функций [00:05]
Для функций есть только полная специализация:
```
template<> void swap(Foo &a, Foo &b) { ... }
```
Частичной нет.
Вместо неё предполагается использовать перегрузки, если очень надо:
```
template<typename T> void swap(vector<T> &a, vector<T> &b) { ... }
```
По техническим причинам это не всегда хорошо, правда, но таков факт жизни.

# Прокси-объекты [00:15]
## Для `vector<bool>` [00:10]
Если мы делаем битовую упаковку в `vector<bool>`, то мы больше не можем вернуть `bool&`.

Но надо вернуть что-то такое, чтобы `a[i] = false;` работало.
В C++03 и раньше хорошей идеей были прокси-объекты:

```
template<>
struct vector<bool> {
private:
    struct vector_bool_proxy {
        vector_bool_proxy operator=(bool value) {
            // Установка бита.
        }
        operator bool() {
            return // Чтение бита.
        }
        // А ещё надо operator</operator== для bool по-хорошему, брр...
    };
public:
    vector_bool_proxy operator[](std::size_t i) { return vector_bool_proxy(....); }
    bool operator[](std::size_t i) const { return ....; }
};
```

## Проблемы [00:05]
Начиная с C++11, как только передали в `auto`: `auto x = v[10];`, ой, не `bool`.

Если были forwarding-ссылки: можно использовать хитрость и писать `auto&&`
