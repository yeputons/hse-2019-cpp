# Шаблонный стэк с гарантиями исключений и превыделенной памятью [00:25]
## Напоминание про стэк и предвыделенную память [00:02]
* Пишем стэк, как в `19-200218`:
  ```
  /*template<typename T>
  struct my_stack {
  private:*/
      T *data;
      std::size_t len, cap;
  /*public:
      my_stack() noexcept;
      my_stack(const my_stack&);
      my_stack& operator=(const my_stack&);
      ~my_stack();
      bool empty() const noexcept;
      std::size_t length() const noexcept;
  */
      void reserve(std::size_t newcap);
  /*    void push(const T&);
      void pop();
      T top() const;
      // Ещё есть empty(), length()
  };*/
  ```
* Проблема: может не быть `T()`, тогда не сработает `data = new T[cap]`.
  Например, у `Employee` (`Developer`, `SalesManager`).
* Надо сначала выделить кусок памяти из байт, а потом руками вызывать конструкторы и деструкторы.
  Для этого есть placement new и явный вызов деструктора.

## Обновление конструктора копирования [00:05]
```
 #include <cstdlib> // Для aligned_alloc
 #include <memory> // Чтобы не было ошибки "no matching function to call to 'operator new(sizetype, void*&)'"
stack(const stack &other) : data(static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T) * other.len])), len(other.len), cap(other.len) {
    // TODO: проверить, что data - не nullptr, иначе выбросить bad_alloc.
    // Или просто new char[] и забить на выравнивание, заодно можно `unique_ptr<char[]>` использовать.
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
Выравнивание по-хорошему надо (например, для атомарных переменных),
по факту `malloc` может быть достаточно хорош.

Держать на доске!

## Обновление reserve и оператора присваивания [00:05]
```
void reserve(std::size_t newcap) {
    /* unique_ptr<T/*[]*/> *newdata = static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T) * newcap));
    // Точный код из конструктора копирования, только в newdata.
    for (size_t i = len; i > 0; i--)
        data[i - 1].~T();
    std::free(data);
    data = newdata.release();
    cap = newcap;
}
```
На самом деле `unique_ptr` тут нельзя, потому что он будет удалять через `delete`/`delete[]`,
а не через `std::free`.
Так что либо `new char[]`, либо свой deleter в `unique_ptr`.

Оператор присваивания: либо copy-and-swap, либо заморачиваемся и пишем почти как в `reserve`.
Это третий раз копипаст кода из конструктора копирования.

## Вынос `stack_impl` [00:13]
Можно сделать то же самое, но лучше, если вынести одно из двух:
* В отдельную функцию код из конструктора копирования, который копирует в
  предвыделенный буфер данные.
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
        T *data;  // unique_ptr нельзя, потому что там не T[].
        size_t len = 0, cap;
        stack_impl(size_t _cap = 0)
            : data(static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T) * other.len]))
            , len(0)
            , cap(cap) {
            // TODO: проверить, что data - не nullptr, иначе bad_alloc.
            //       Или просто new char[].
        }
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


## Move-семантика в стеке [00:05]
* `reserve` в стеке
  * При перевыделении буфера можно делать move.
  * Если есть noexcept move для элементов (что вероятнее noexcept copy), то будет строгая гарантия (в домашке надо).
  * А вот если нет noexcept, то будет лишь базовая гарантия.
    Если в процессе перетаскивания вылетело исключение, то имеем два покоцанных буфера и таскать ничего не можем.
* method ref qualifier
  * А ещё можно перегрузить `top()` для временного объекта:
    `T top() && { return std::move(h.data[0]); }` (тут уже `move` обязателен).
    Тогда будет работать `Foo x = getStackOfFoo().top();`
    Это значит "вызывайся только на rvalue this".
  * А ещё можно запретить вызывать `push()` на временном объекте.
    Сейчас `stack().push(10)` сработает, хотя это бесполезно.
    `void push(T) &`
    Это значит "вызывайся только на lvalue this".
  * Если не писать ни того, ни другого, будет вызываться на любых `this`.
