# Шаблонный стэк с гарантиями исключений и превыделенной памятью [00:30]
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

## Вынос `stack_impl` [00:10]
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
