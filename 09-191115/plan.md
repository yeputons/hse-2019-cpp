# Класс, управляющий ресурсами [00:40]
## Класс без ресурсов [00:05]
```
struct Array {
private:
    vector<int> data;
public:
    Array(size_t init_len = 0) : data(init_len) {}
};
```

Обычно не надо вообще ничего писать: используем внутри только вектора, всё автоматически создаётся, копируется, удаляется.
Ну, можно конструктор создать.

## Напоминание [00:10]
```
struct Array {
private:
    int* data;
    size_t len;
public:
    Array(size_t init_len = 0)
        : data(new int[init_len])
        , len(init_len) {
    }
    ~Array() {
        delete[] data;
    }
    // Что ещё?
};
```

## Правило трёх, copy-and-swap [00:10]
Правило трёх: из destructor, copy constructor, assignment operator вы реализуете либо ноль, либо все три.
Почему: если потребовалось, то у вас наверняка есть какой-то ресурс.
В C++11 расширяется до правила пяти.

Надо ещё точно написать конструктор копирования для copy initialization:
    ```
    Array(const Array &other)
        : data(new int[other.len])
        , len(other.len) {
        for (int i = 0; i < len; i++)
            data[i] = other.data[i];
    }
    ```

А дальше вместо оператора присваивания можно использовать copy-and-swap:
    ```
    void swap(Array &other) {
        using std::swap;
        swap(a.data, b.data);
        swap(a.len, b.len);
    }
    Array& operator=(Array other) {
        this->swap(other);
        return *this;
    }
    ```
Скорость работы похожая, не надо разбирать случай `this == &other`.

## Оптимизации copy initialization [00:15]
* RVO: `Array createArray() { return Array(10); }` (обязательно с C++17)
  Зачем: `Array x = createArray();`, копировать не надо.
* NRVO: `Array createArray(      ) { Array a(10);              return a; }` (необязательный NRVO)
  Зачем: `Array x = createArray();`, копировать не надо.
  В чём проблема: `createArray(bool x) { Array a(10); Array b(20); return x ? a : b; }
* Copy elision: можно выкинуть копию, даже если есть побочные эффекты от копирования.
  Не только рядом с `return`: `print(Array(10));`.
  Это одно из двух мест (судя по cppreference), когда оптимизатор _по стандарту_ может менять поведение
  программы, а не просто из-за UB.

# Перегрузки [00:20]
## Напоминание [00:08]
* Перегрузка - выбор наиболее подходящей по типу функции на этапе компиляции.
  ```
  void print(int x) { ... }
  void print(float x) { ... }
  void print(const char *x) { ... }
  print(10); // 1
  print(10.0f); // 2
  print("10"); // 3
  ```
* Возможны ambiguity:
  ```
  print(10.0); // 1 или 2?
  ```
* Обычно работает "как ожидается", если типы совсем разные.
* Но бывают эффекты с числами и прочей конвертацией:
  ```
  print('x'); // 1, потому что можно сконвертировать
  ```

  NULL, nullptr, 0

## Const qualifier [00:07]
Работают аналогично с методами:

```
int& operator[](size_t id) { return data[id]; }
int operator[](size_t id) const { return data[id]; }

// operator[](Array*, size_t) -> int&
// operator[](const Array *, size_t) -> int
```

## Параметры по умолчанию [00:05]
* Можно указывать у методов и кого угодно, вычисляются в момент _вызова_:
  ```
  void foo(int x = 10);

  int main() {
      foo(); // --> foo(10);
  }
  ```
* Добавляют перегрузки:
  ```
  void foo(char);
  void foo(int, int x = 10);
  foo('1'); // 1
  foo(1); // 2
  foo(1.0); // ambgious
  ```

# Неявные преобразования copy initialization [00:10]
* Выглядит, как неявное преобразование:
  ```
  void print(Array a) { ... }
  int main() {
      Array a(10);
      Array b = 10;
      print(a);
      print(10);
  }
  Array f() { return 10; }
  ```
* Более того: можно добавить перегрузку `print(int)`, тогда
  начнёт вызываться `print(int)`.
  Потому что подходит больше и требует меньше преобразований.
* Такой вызов - это странно. В C++11 можно отключить конструктор для копирующей инициализации `explicit Array(int n)`.
  Тогда в copy initialization и неявной конвертации использовать нельзя.

# Const и mutable [00:10]
* В C++ `const` действует даже внутрь всех объектов:
  ```
  struct Foo { int x = 100; };
  struct Bar { vector<Foo> v; };
  const Bar b;
  b.v[0].x = 10;
  // b // const Bar&
  // b.v // const vector<Foo>&
  // b.v[0] // const Foo&
  // b.v[0].x // const int&
  ```
* Можно и нужно добавлять к member-функциям:
  ```
  size_t length() const { return ... }
  ```
  * Ещё можно захотеть какие-то
* В параметрах обычно используется вместе со ссылкой:
  ```
  // Конструктор копирования
  Array(const Array &a) {}
  ```
* Тонкость: в `const Array &a` можно положить ссылку на временный объект:
  А в обычную ссылку нельзя.
  Это полезно в параметрах метода:
  ```
  Array x = createArray(); // Конструктор копирования.
  ```
* И иногда даже с переменными:
  ```
  const Array &a = Array();
  ```
  Тогда время жизни объекта расширится.
  Но если он был создан где-то ещё, то упс:
  ```
  const Array &a = max(Array(), Array());
  ```
Временные объекты: время жизни до конца `;`
mutable
