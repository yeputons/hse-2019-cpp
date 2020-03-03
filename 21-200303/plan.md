# Move-семантика — реализация [00:30]
## Категории значений [00:10]
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
    * `this`
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

## rvalue-ссылки [00:05]
* C++11 добавляет новый вид ссылки: rvalue-ссылка: `Foo&& x = foo();`
* Это работает точно так же, как обычная константная ссылка: может биндится только к rvalue (xvalue, prvalue).
* А обычная ссылка может биндится только к glvalue. К prvalue не может.
* `const Foo&&` — хрень, не пишите так. Забиндится, но move не сработает.

## Move-семантика — конструктор перемещения и оператор перемещающего присваивания [00:10]
* Конструктор перемещения — просто перегрузка:
  ```
  stack(const stack &other) : data(new ...), ... {}
  stack(/*const, WTF */ stack &&other) /*noexcept*/ : data(std::move(other.data)), cap(std::move(other.cap)), len(std::move(other.len)) {} {
      // Здесь важно, что std::move(other.data), потому что это unique_ptr.
      // А дальше важно занулить cap/len. std::move — это просто смена категории. Для примитивных типов ничем от копирования не отличается.
      other.cap = other.len = 0;
  }
  ```
* Аналогично с оператором присваивания:
  ```
  stack& operator=(stack &&other) /* noexcept */ {
      if (this == &other) return *this;
      data = std::move(other.data);
      cap = other.cap;
      len = other.len;
      other.cap = other.len = 0;
      return *this;
  }
  ```
  Или можно просто `swap`. Нам же необязательно оставлять `other` пустым.
* И тут copy-and-swap позволяет нам не реализовывать два оператора:
  ```
  stack& operator=(stack other) {
      swap(data, other.data);
      return *this;
  }
  ```

## Move-семантика в стеке [00:05]
* Осторожно с universal reference (с ними будем потом):
  * `template<typename T> foo(T&& x) {}` // move не ок; может схватить и lvalue.
  * `template<typename T> foo(vector<T>&& x) {}` // move ок, схватит только rvalue.
