# Долг [00:05]
* 08.11.2019: value-initialization на самом деле не делает zero-initialization структуры!
  С C++11: Для интов делает, и если нет явного конструктора по умолчанию, тоже делает.
  ```
  struct Foo {
      int x;
      // Foo() {} // поле станет default-initialized
      // Foo() : x() {} // поле станет value-initialized
  };
  Foo f = Foo(); // value-initialization + copy-initialization elision, будет ноль.
  printf("%d\n", f.x); // Компилятор выдаст предупреждение
  ```

# Мелочи [00:25]
* `= delete`
    * `MyFile(const MyFile&)` = delete вместо приватного конструктора копий.
* `= default`
    ```
    MyPoint(const MyPoint) = default
    MyPoint& operator=(const MyPoint&)
    MyPoint() = default вместо MyPoint() {}
    ```
    * Чтобы не писать руками копирование членов. Можно и вообще не писать, само появится (move-семантики-то ещё нет), но так можно обозначать намерение.
* `final` у метода
    ```
    struct Foo { virtual void f() {} };
    struct Bar : Foo { void f() override final { }};
    struct Baz : Bar { void f(); /* ошибка компиляции даже без override */ };
    ```
* `final` у класса
    ```
    struct Foo { };
    struct Bar final : Foo { };
    struct Baz : Bar {}; // ошибка компиляции
    ```
* Инициализация полей в классе вместо member initialization list
    * Было: `struct Foo { int x; Foo() : x(10) {} };`
    * Стало: `struct Foo { int x = 10; };`
    * Можно и переопределить: `struct Foo { int x = 10; Foo(int y) : x(y) {} };`
* Делегирующие конструкторы
    ```
    struct Foo {
        int x;
        Foo(int y) : x(y) {}
        explicit Foo(const char *s) : Foo(atoi(s)) {}
    };
    ```
* Реализация чисто виртуальной функции
    ```
    struct Foo {
        virtual void f() = 0;
    }
    void Foo::f() { std::cout << "hi"; }
    ```
    * Можно будет потом явно вызвать: Foo::f().
    * Или в конструкторе.

* public/protected/private-наследование:
    ```
    struct Foo { int x; protected: int y; };
    struct BarPublic : /*public*/ Foo { /* public x, protected y */ };
    struct BarProtected : protected Foo { /* protected x, protected y */ };
    struct BarPrivate : private Foo { /* private x, private y */ };
    class BarPrivate2 : /*private*/ Foo { /* private x, private y */ };
    struct Baz1 : BarProtected { void f() { y = 10; } }; // ok
    struct Baz2 : BarPrivate { void f() { y = 10; } }; // compilation error
    ```
    * Обычно надо делать public-наследование, private надо очень редко.

# Время жизни [00:10]
* Правило трёх и нуля
    * Из destructor, copy constructor, assignment operator вы реализуете либо ноль, либо все три. Лучше ноль.
    ```
    struct Foo {
        int *x;
        int *y;
        ~Foo() { delete[] x; delete[] y; }
    };
    ```
    лучше с `vector<int>`.
* Термин "lifetime":
    * Объект "живёт" только между конструктором и деструктором, даже если байты лежат в памяти.
    * На "мёртвом" объекте нельзя вызывать методы, даже если они не обращаются к полям — UB. В частности, второй раз деструктор. Даже если он ничего не делает. Даже если в классе полей нет.
    * Поэтому нельзя явно вызывать деструктор на объекте с automatic storage duration (на стеке).

# Placement new [00:10]
* "Время жизни объектов != выделена память".
  Можно собирать объект там, где раньше лежал `char`:
  Placement new:
  ```
  void *data = new char[100];
  Foo *foo = new(data) Foo;
  // lifetime of `foo`
  foo->~Foo();
  delete[] data;
  ```
* Это используется в векторе, чтобы не перевыделять память.
* Что будет, если закомментировать какую-нибудь строчку?
* А если создать второй объект поверх старого? Окей, только деструктор не вызовется.
  И будут дикие проблемы, если внутрь положить `const`, но это уже мелочи:
  ```
  struct Foo { const int x; Foo(...) { ... } }
  char data[sizeof Foo];
  Foo *f1 = new(data) Foo(10);
  f1->x; // ok
  Foo *f2 = new(data) Foo(20);
  f1->x; // not ok
  f2->x; // ok???
  ```

# ООП vs процедурное [00:15]
* Термины "инкапсуляция", "наследование", "полиморфизм"?
    * Инкапсуляция — можно не думать про детали реализации, а пользоваться готовым интерфейсом. Пример: IntArray, сам управляет памятью.
    * Наследование — техническая возможность расширять готовый класс. Но для этого хорошо бы, чтобы базовый класс был к этому готов. Применять осторожно, лучше композицию. Примеров нет.
    * Полиморфизм — когда появились виртуальные функции, можем писать один код, который работает с разными наследниками. Пример: figures.

## Сортировка [00:05]
```
// C style
void qsort (void* base, size_t num, size_t size, int (*compar)(const void*,const void*)){}
class Comparable{
    //or virtual bool operator<(const Comparable *v) = 0 const;
    virtual int compare(const Comparable* v) = 0 const;
};
// OOP style
void nsort(Comparable** m, size_t size) { }
class Point: public Comparable {
private:
  int x, y;
public:
  virtual int compare(const Comparable* v) const {...};
};
```
