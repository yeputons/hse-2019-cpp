# Шаблонный стэк с гарантиями исключений и превыделенной памятью [00:35]
## Напоминание про стэк и предвыделенную память [00:05]
* Пишем стэк, как в `19-200218`:
  ```
  template<typename T>
  struct my_stack {
  private:
      T *data;
      std::size_t len, cap;
  public:
      my_stack() noexcept;
      my_stack(const my_stack&);
      my_stack& operator=(const my_stack&);
      ~my_stack();
      void reserve(std::size_t newcap);
      void push(const T&);
      void pop();
      T top() const;
      // Ещё есть empty(), length()
  };
  ```
* Проблема: может не быть `T()`.
  Например, у `Employee` (`Developer`, `SalesManager`).
  Тогда `new T[10]` не сработает.
* Надо сначала выделить кусок памяти из байт, а потом руками вызывать конструкторы и деструкторы.

## Напоминание про placement new и деструкторы [00:05]
* В отличие от прошлой лекции, лучше сразу скастовать указатель к `T*`, будет удобнее:
  ```
  #include <cstdlib> // Для aligned_alloc
  #include <memory> // Чтобы не было ошибки "no matching function to call to 'operator new(sizetype, void*&)`"
  struct Foo { Foo(int) {} };
  int main() {
      T *data = static_cast<Foo*>(std::aligned_alloc(alignof(Foo), sizeof(Foo) * 3));  // Или std::malloc(sizeof(Foo) * 3);
      Foo *a = new (data) Foo(10);  // placement new
      /*Foo *b = new (data + 1) Foo(20);
      Foo *c = new (data + 2) Foo(30);
      c->~Foo();
      b->~Foo();*/
      a->~Foo(); // Явный вызов деструктора.
      std::free(data);
  }
  ```
  Выравнивание по-хорошему надо (например, для атомарных переменных),
  по факту `malloc` может быть достаточно хорош.
* А теперь надо везде заменить `new[]` и `delete[]` на вот такое перевыделение памяти.

## Обновление конструктора копирования [00:05]
```
stack(const stack &other) : data(std::aligned_alloc(alignof(T), sizeof(T) * other.len]), len(other.len), cap(other.len) {
    size_t i = 0;
    try {
        for (; i < len; i++)
            // new (data + i) T(other.data[i]);
            data[i] = other.data[i];
    } catch (...) {
        for (; /*i > 0*/ i >= 0; i--)
            data[/*i - 1*/ i].~T();
        std::free(data);
        throw;
    }
}
```
Держать на доске!

## Обновление reserve [00:05]
```
void reserve(std::size_t newcap) {
    /* unique_ptr<T/*[]*/> *newdata = static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T) * newcap));
    // Точный код из конструктора копирования, только в newdata.
    for (size_t i = len; i > 0; i--)
        data[i - 1].~T();
    data = newdata.release();
    cap = newcap;
}
```

## Обновление оператора присваивания [00:02]
Либо copy-and-swap. Либо заморачиваемся и пишем почти как в `reserve`.
Третий раз копипаст.

## Вынос `stack_impl` [00:13]
Можно сделать то же самое, но лучше, если вынести одно из двух:
* Код из конструктора копирования, который копирует в предвыделенный
  буфер данные.
* Отдельный класс `stack_impl`, который хранит `data`, `len`, `capacity`,
  не теряет память и вызывает деструкторы.
  Этот вариант лучше, потому что он позволяет вызывать не только копирование,
  но и просто `N` одинаковых конструкторов (например, по умолчанию).

А тут вызывать только конструкторы.
```
template<typename T>
struct stack {
private:
    struct stack_impl { // Все поля публичные.
        unique_ptr<T> data;
        size_t len = 0, cap;
        stack_impl(size_t _cap = 0)
            : data(std::aligned_alloc(alignof(T), sizeof(T) * other.len])
            , len(0)
            , cap(cap) {}
        // stack_impl(const stack_impl&) = delete;  // Уже удалено из-за unique_ptr.
        // stack_impl& operator=(const stack_impl&) = delete;   // Уже удалено из-за unique_ptr.
        ~stack_impl() {
            for (; /*len > 0*/ len >= 0; len--)
                data[/*len - 1*/ len].~T();
            std::free(data);
        }
    };
    stack_impl h;
};
```

Теперь:
```
stack(const stack &other) : h(other.h.len) {
    while (h.len < other.h.len)
        new (h.data + h.len) T(other.h.data[h.len++]);  // Упс: h.len меняем дважды, а ещё с исключениями неясно. Лучше for
}
void reserve(std::size_t newcap) {
    stack_impl newh(newcap);
    for (; newh.len < h.len; newh.len++)
        new (newh.data + newh.len) T(h.data[newh.len]);
    // swap(newh, h);
}
/*stack& operator=(const stack &other) { // Как в reserve.
    if (this == &other) return *this;
    stack_impl newh(other.len);
    for (; newh.len < other.h.len; newh.len++)
        new (newh.data + newh.len) T(other.h.data[newh.len]);
    // swap(newh, this.h);
    return *this;
}*/
```

# Move-семантика — использование [00:20]
## Проблемы [00:05]
1. Много копирований не по делу:
   ```
   struct Foo {  // Например, сотрудник.
       string a, b;  // Например, имя и фамилия.
       Foo(string /*const&*/_a, string _b) : a(_a), b(_b) {}
   };
   // ...
   string readString() {
       string result;
       cin >> result;
       return result;
   }
   // ...
   vector<Foo> v = ...;
   std::string a = readString();
   std::string b = readString();
   v.push_back(Foo(a, b)); // Переменные хочу, чтобы явно указать порядок.
   ```
   NRVO-то поможет (хотя это и сложно), а вот дальше никак.
   Копируем и в конструктор `Foo` (хотя там можно ссылку вставить), и в поле,
   а потом ещё и сам `Foo`.
2. `unique_ptr` нельзя копировать, но надо возвращать из функций.
   Надо "гарантированное" RVO или что-то похожее.

## Решение [00:05]
1. Скажем, что объекты можно не только копировать (copy), но и перемещать (move).
   Если (`a <-- b`) переместили `b` в `a`, то:
   * В `a` теперь то, что лежало раньше в `b`
   * В `b` теперь непонятно что (moved from), но что-то очень корректное.
     Не просто деструктор вызовется, а можно все операции без предусловий вызывать (size, push_back, clear).
     Не UB.
2. Технически для этого придумываются конструктор перемещения (`string(string&&)`) и
   оператор перемещающего присваивания (`string& operator=(string&&)`), вдобавок к "правилу трёх",
   получается "правило пяти".
   Здесь `Foo&&` — обозначение "сюда только временные значения", к ним ещё вернёмся.
   В стандартной библиотеке такое есть у всех.
   У кого-то даже сказано, что такое "moved from state" (например, у `unique_ptr` это пустое).
   При правиле нуля и примитивным типам такое не надо.
3. А дальше компилятор автоматически понимает, кто временный, а кто нет.
   Если просто взяли программу и скомпилировали с C++11, move-семантика уже заработала.
   Правило нуля автоматически включит move-семантику для пользовательских типов.
   STL-контейнеры работают.
   Всё ускорится.
4. Мы можем подсказать (обязательно в случае `unique_ptr`):
   ```
   std::string a = readName();
   std::string b = readName();
   v.push_back(Foo(std::move(a), std::move(b)));
   ```
5. Обращаю внимание, что `std::move` лишь обозначает значение как временное,
   но перемещение не делает.
   Так что он даже обратно совместим.

Ещё пример:
```
template<typename T>
void swap(T &a, T &b) {
    T t = std::move(a);
    a = std::move(b);
    b = std::move(t);
}
```

## Параметры функций [00:05]
Если функция что-то копирует:
```
void push(const T &other) {
    new (data + len) T(other);  // Компилятор не имеет права портить `other`, он же константный.
    len++;
}
//
push(readName());  // Копии не по делу! Одна при возврате, одна при конструировании. И одно удаление.
```
то можно добавить перегрузку, станет эффективнее:
```
void push(T&& other) {
    new (data + len) T(/*std::move*/ other);  // std::move обязателен, other — lvalue.
                                              // Тут есть обратная совместимость, даже если `T` не умеет в move, то будет копия из ссылки.
    len++;
}
```

А ещё проще написать не два раза, а принять параметр по значению, а не по константной ссылке.
Конструкторы параметра разберутся:
```
void push(T other) {
    new (data + len) T(/*std::move*/ other);
    len++;
}
```

Но тут есть тонкости с вызовами деструкторов, это не всегда лучше.
Детали на практике и в:

* https://stackoverflow.com/questions/51705967/advantages-of-pass-by-value-and-stdmove-over-pass-by-reference
* https://stackoverflow.com/questions/26261007/why-is-value-taking-setter-member-functions-not-recommended-in-herb-sutters-cpp

## Как использовать move-семантику [00:05]
1. Правило нуля при реализации своих классов.
2. Если мы передаём кому-нибудь переменную, которая больше нам не нужна,
   оборачиваем её в `std::move()`.
3. Если функция что-то копирует, то __обычно__ принимает параметр по значению, а не по константной ссылке.

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
* Обычно move не кидает исключений. Сам конструктор/оператор присваивания.
  Это полезно и гораздо чаще noexcept copy.
* Итого move-семантика в своих классах:
  * Правило пяти вместо правила трёх. И лучше правило нуля :)
  * `std::move` уже нужен почаще.
  * `move` можно просто выразть через `swap`.
    В конструкторе — с пустым.

## Move-семантика в стеке [00:05]
* Осторожно с universal reference (с ними будем потом):
  * `template<typename T> foo(T&& x) {}` // move не ок; может схватить и lvalue.
  * `template<typename T> foo(vector<T>&& x) {}` // move ок, схватит только rvalue.
* `reserve` в стеке
  * При перевыделении буфера можно делать move.
  * Если есть noexcept move для элементов (что вероятнее noexcept copy), то будет строгая гарантия (в домашке надо).
  * А вот если нет noexcept, то будет лишь базовая гарантия.
    Если в процессе перетаскивания вылетело исключение, то имеем два покоцанных буфера и таскать ничего не можем.
