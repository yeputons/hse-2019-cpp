# Долги
А вот `template<typename T, typename U> void foo(U x);` и `foo<void>(10)`
уже вызовется: `T=void` явно, `U` вывели сами.

`template<typename T, typename Cmp> void sort(Array<T> &arr, Cmp cmp);`
И нет никаких виртуальных вызовов или чего-то такого.
В целом есть двойственность между шаблонами и виртуальными вызовами,
это разные способы диспетчеризации.

Тип лямбды неизвестен, его явно не указать, только автовыводом.

# Мелочи шаблонов [00:30]
## typedef, using [00:04]
* Инстанцированный шаблон — это тип, можно сделать `typedef`: `typedef vector<int> vi;`
* Неинстанцированный — нельзя: `typedef vector v;` не работает.
* Два костыля:
  * `#define vector v` — сразу проблема, потому что разные namespaces, имена переменных...
  * `template<typename T> struct v : vector<T> {};` — это всё-таки разные типы получаются, надо добавлять какие-то операторы преобразования из `v` в `vector`.
* Нормальный способ: `template<typename T> using v = vector<T>`, называется alias template.
  * Можно и хитрое: `template<typename T>

## Много параметров, параметры по умолчанию [00:03]
```
template<typename T, typename U = void, typename V = int>
struct T { ... };
T<char> x; // T<char, void, int>
T<char, char> y; // T<char, char, int>
T<char, char, char> z; // T<char, char, char>
```
С функциями тоже работает.
Только там ещё есть автовывод типов.

## Параметры-значения [00:03]
```
template<typename T, std::size_t N /* = 10 */>
struct my_array {
    T arr[N];
    std::size_t size() const { return N; }
};
my_array<int, 10> arr;
```
Можно указывать простые параметры: целые числа (`bool` тоже), любые указатели и ссылки на статические переменные.
Нельзя указывать `std::string` и сложные типы.
С С++20 разрешили какие-то структуры, вещественные числа.

Снова возникают тонкости с парсингом: `foo<0>1>` надо писать как `foo<(0>1)>`.

## Параметры-шаблоны [00:05]
```
template<typename T, template<typename> typename Container = std::vector>
struct priority_queue {
    Container<T> c;
    ...
};
priority_queue<int>  // priority_queue<int, std::vector>
priority_queue<int, priority_queue>  // struct priority_queue<int, priority_queue> { priority_queue<int> c; }
```

### Шаблонные методы в шаблонных классах [00:05]
```
template<typename T>
struct shared_ptr {
    Storage *s;
    shared_ptr(T *data);
};
struct Base {};
struct Derived : Base {};
void foo() {
    shared_ptr<Derived> d = ...;
    shared_ptr<Base> b = d;  // Упс, несовместимые типы.
}
```

Решение:
```
template<typename T>
struct shared_ptr {
    ...    
    template<typename U>
    shared_ptr(shared_ptr<U> p);
};
```

При инстанцировании проверит, что так можно инициализировать указатели.
В частности, не разрешит из `Derived*` получить `Base*` или `int`.

Можно не только с конструкторами, но и с методами.

## Вложенные типы/значения и слова `typename`/`template` [00:05]
```
template<typename T> struct A { ... };
template<typename T> struct B {
    void foo() {
        /* typename */ A<T>::x * y;
    }
};
```
Это либо умножение, либо объявление переменной типа `T::x`.
Пока не посмотрим в шаблон — не узнаем.
А ещё бывают специализации (будут потом), так что в зависимости от `A` это может быть разное.

Некоторые компиляторы требуют, чтобы вы явно писали слово `typename` перед заким зависимым типом:
`typename A<T>::x * y`, иначе они будут трактовать `x` как переменную.
А если написать это как поле класса, то выдадут ошибку.

Более сложный пример:
```
template<typename T> struct A { int x; };
template<typename T> struct B {
    template<typename U> using C = A<U>;
};
template<typename T> struct D {
    typename B<T>::template C<T>::x * y;
};
```

Аналогичная проблема может возникнуть при вызове шаблонных функций, которым
хотим явно задать параметры:
```
template<typename T> struct Foo {
    template<typename U> void foo();
};
template<typename T>
void bar() {
    Foo<T> x;
    x./*template */foo<char>();
}
```

## Автовывод параметров типа (C++17) [00:05]
```
template<typename T1, typename T2> struct pair { T1 first; T2 second; }
template<typename T1, typename T2> pair<T1, T2> make_pair(T1 a, T2 b) {
    return pair<T1, T2>(a, b);
}
template<typename T1, typename T2>
foo(pair<T1, T2> x);
void bar() {
    foo(pair<int, int>(10, 10)); // ок
    foo(make_pair(10, 10));  // ок, автовывод
    foo(pair(10, 10)); // не ок до C++17, неизвестен тип.
}
```
Начиная с C++17 есть Class template argument deduction, который пытается
перебрать конструкторы и решить уравнение на типы класса.
Можно добавлять и свои уравнения, но мы не будем.

# Шаблонный стэк с гарантиями исключений [00:40]
## Постановка задачи [00:05]
Хотим написать:
```
template<typename T>
struct stack {
private:
    T *data;
    size_t len, cap;
public:
    stack() : data(nullptr), len(0), cap(0) {}
    /* stack(const stack&); */
    ~stack();
    bool empty() / *const */ { return len == 0; };
    size_t length() /* const */ { return len; };

    void reserve(size_t newcap);
    /* stack& operator=(const stack&); */

    void push(T /* const T& */);
    T pop();
};
```

При этом хотим строгую гарантию исключений для всех операций.
* Правило трёх, тут везде строгие гарантии:
  ```
  template<typename T> stack::~stack() { delete[] data; }  // Даже если data == nullptr
  template<typename T> stack::stack(const stack &other) : data(new data[other.len]), len(other.len), cap(other.len) {
      for (size_t i = 0; i < len; i++)
          data[i] = other.data[i];
      /* catch (...) { delete[] data; throw; } /* мы же в конструкторе; или заменить на unique_ptr */ */
  }
  template<typename T> stack<T>& /*auto*/ stack::operator=(stack other) {
      swap(other);
      return *this;
  }
  ```
* ```
  template<typename T> void stack::reserve(size_t newcap) {
      if (newcap < cap) return;
      /* unique_ptr */ T *newdata = new T[newcap];
      for (size_t i = 0; i < len; i++)
          newdata[i] = data[i];
      delete[] data;
      data = newdata;
      cap = newcap;
  }
  ```

## Сложности с `operator=` [00:05]
* Без copy-and-swap:
  ```
  template<typename T> stack<T>& stack::operator=(const stack &other) {
      if (this == &other) return *this;  // Необязательно.
      reserve(other.len);
      for (size_t i = 0; i < other.len; i++)
          data[i] = other.data[i];  // Если кинуло, то упс.
      len = data[i];
      return *this;
  }
  ```
  Правильнее (никогда не портим старые данные до самого конца):
  ```
  template<typename T> stack<T>& stack::operator=(const stack &other) {
      if (this == &other) return *this;  // Обязательно!
      /* unique_ptr<T/*[]*/>, иначе утечка */ T *newdata = new T[other.len];
      for (size_t i = 0; i < other.len; i++) // Можно вынести в функцию, чтобы не дублировать код.
          newdata[i] = other.data[i];
      delete[] data;  // Не должно кидать.
      data = newdata;
      len = cap = other.len;
      return *this;
  }
  ```

## Сложности с `pop` [00:05]
* ```
  template<typename T> void push(const T &value) {
      reserve(len + 1);
      data[len++] = value;
  }
  ```
* ```
  template<typename T> T pop() {
      return data[len--];
  }
  ```
  Проблема: если при копировании элемента вылетело исключение, то он уже убран из стэка.
  До C++11 тут ничего разумного не сделать.
  Поэтому обычно разделяют на `void pop()` и `T top()`.

## Placement new, ручной вызов деструктора, выравнивание памяти [00:05]
* Проблема: может не быть `T()`.
  Например, у `Employee` (`Developer`, `SalesManager`).
  Тогда `new T[10]` не сработает.
* Надо сначала выделить кусок памяти из байт, а потом руками вызывать конструкторы и деструкторы.
* ```
  #include <memory> // Чтобы не было ошибки "no matching function to call to 'operator new(sizetype, void*&)`"
  struct Foo { Foo(int) {} };
  int main() {
      void *data = aligned_alloc(alignof(Foo), sizeof(Foo) * 3);  // Или malloc(sizeof(Foo) * 3);
      Foo *a = new (data) Foo(10);  // placement new
      Foo *b = new (static_cast<char*>(data) + sizeof(Foo)) Foo(20);
      Foo *c = new (static_cast<char*>(data) + sizeof(Foo) * 2) Foo(30);
      c->~Foo(); // Явный вызов деструктора.
      b->~Foo();
      a->~Foo();
      std::free(data);
  }
  ```
  Выравнивание по-хорошему надо (например, для атомарных переменных),
  по факту `malloc` может быть достаточно хорош.
* А теперь надо везде заменить `new[]` и `delete[]` на вот такое перевыделение памяти.

## Обновление конструктора копирования [00:05]
```
template<typename T> stack::stack(const stack &other) : data(aligned_alloc(alignof(T), sizeof(T) * other.len]), len(other.len), cap(other.len) {
    // TODO: проверить, что data - не nullptr, иначе выбросить bad_alloc.
    size_t i = 0;
    try {
        for (; i < len; i++)
            data[i] = other.data[i];
    } catch (...) {
        for (; i > 0; i--)
            data[i - 1].~T();
        free(data);
        throw;
    }
}
```

## Обновление оператора присваивания [00:10]
TODO

## Вынос `stack_impl` [00:05]
Можно сделать то же самое, но лучше, если вынести отдельный класс `stack_impl`, который хранит `data`, `len`, `capacity`,
не теряет память и вызывает деструкторы.
А тут вызывать только конструкторы.

TODO
