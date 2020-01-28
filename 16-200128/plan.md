# Долги [00:20]
## Где читать подробности и про дизайн [00:05]
Два полезных ресурса.
На экзамене и практиках не надо знать наизусть, я сам полностью не читал, но там явно пишут хорошее.
Всё на английском, на русском не искал.

* [ISO C++ FAQ](https://isocpp.org/wiki/faq) — высокоуровневое описание фич и проблем с примерами.
  * Отвечает хорошо, подробно, и с правильной терминологией из стандарта.
  * Но не рассказывает про вообще все фичи, концентрируется на "правильном в современном C++".
  * "What’s the difference between the keywords struct and class?"
  * "Why is the size of an empty class not zero?"
  * "What if I want a local to “die” before the close } of the scope in which it was created? Can I call a destructor on a local if I really want to?"
  * "Why are destructors not virtual by default?"
  * "When should someone use private virtuals?"
* [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
  * Набор правил для хорошего кода на современных плюсах (С++17).
  * Всё ещё высокоуровнево, но это уже структурированные рекомендации, а не сборник вопросов.
  * Примеры категорий: "R: Resource management".
  * Пример правила: "R.1: Manage resources automatically using resource handles and RAII (Resource Acquisition Is Initialization)"
  * Пример правила: "R.10: R.10: Avoid malloc() and free()"

## Скрытие родительских методов [00:12]
* Бывает override виртуального метода, но по умолчанию в нас везде hiding.
  https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule
* Если в ребёнке объявлен хотя бы один метод с именем `foo`,
  то в родителей не смотрим вообще.
  Если там были методы с таким же именем (даже override), то упс.
* Так что не стоит добавлять перегрузки так просто:
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
  b.Foo::foo(1); // int
  // На засыпку
  Foo &b2 = b;
  b2.foo(1.2); // int
  b2.foo(1); // int
  ```
* Конструкция `using` "добавляет" все имена методов в текущий класс.
* А вот если бы написали `void foo(int);`, то это было бы объявление
  нового метода `Bar::foo`.
  * Выглядит как override, но виртуальности-то нет.
* Вывод: как только в иерархии перегрузка — упс.
* Вывод: как только в иерархии разные методы без `virtual` — думать.

## Изменение видимости метода [00:03]
* `using` можно использовать жутким образом:
  ```
  struct Foo {
  private:
      void magic();
  };
  struct Bar : Foo {
      using Foo::magic;
  };
  Bar b;
  b.magic();  // Foo::magic()
  ```
* Обычно не надо.
* Если очень хочется, то можно так добывать приватные методы для тестирования.

# Множественное наследование [00:25]
## Основы [00:10]
* ```
  struct PieceOfArt { std::chrono::time_point date; }
  struct Music : PieceOfArt { ... };
  struct Lyrics : PieceOfArt { ... };
  struct Song : Music, Lyrics {
      Song(...) : Music(...), Lyrics(...), album(...) {}
      std::string album;
      // using Music::date;
  };
  Song s;
  x = s.Music::date;
  y = s.Lyrics::date;
  ```
* Memory layout: просто написали несколько подряд.
  Возможно, с повторами.
* Если возникает пересечение по именам, добавляем квалификатор
  (внутри тоже работает).
* Порядок инициализации — dfs post-order слева направо.
  Смотрим на порядок в объявлении класса, не в initializer list.
* Порядок уничтожения — обратный.

## Тонкости memory layout [00:15]
* Касты теперь меняют указатель: `Song` и `Lyrics`.
* Ambiguous base (даже для приватных): к `PieceOfArt` кастовать напрямую нельзя.
  Надо сначала к `Song` или к `Lyrics`.

```
struct A {};  // 1
struct B : A {};  // 1
struct C : A {};  // 1
struct D : B, C {};  // 2
```

* Тонкость: можно даже без полей получить класс большого размера,
  потому что подобъекты должны иметь разные адреса
  (если они не наследуются).
* Есть empty base optimization:
  https://en.cppreference.com/w/cpp/language/ebo

# Виртуальное наследование [00:30]
## Основы [00:10]
* Иногда две экземпляра базового класса — это странно.
  ```
  struct Person { std::string name; }
  struct Student : /* virtual */ Person {};
  struct Employee : /* virtual */ Person {};
  struct MagicStudent : Student, Employee {};
  ```
* Нарисовать стрелочки на доске.
  Можно некоторые базы назвать виртуальными.
  Тогда они начнут склеиваться (перерисовать стрелочки на доске).
* Но кто инициализирует `Person`? Самый вложенный класс.
  ```
  MagicStudent(...) : Person(...), Student(...), Employee(...) {}
  ```
  а запись `: Person(...)` в `Student`/`Employee` игнорируется.
* При этом каждый конструктор в иерархии, кроме чисто виртуальных
  классов, должен вызывать `Person` (или `Person` должен иметь конструктор по умолчанию),
  иначе компилятор не может сгенерировать код конструктора.
* Важно: про виртуальность надо говорить _сразу под_ тем, кто станет виртуальным.
  Не в самом конце решать.

## Memory layout [00:10]
* Сначала все виртуальные, потом как обычно.
* Но как тогда `Student`/`Employee` знает, где лежит его `Person`?
  Ещё один указатель в начале структуры, показывает на виртуальную базу.
* Итого получаем, что каждое слово `virtual` в иерархии — плюс указатель,
  даже если на самом деле не надо.

## Использование для интерфейсов [00:10]
* Виртуальные классы могут содержать поля, у меня даже
  такой случай был на стажировке, но обычно нет.
* Частое использование — ABC (абстрактные базовые классы), они же интерфейсы.
* Интерфейc — это класс, в котором нет никаких данных, одни виртуальные функции.
  ```
  struct IPerson {
      virtual std::string getName() const;
  };
  struct IStudent : virtual IPerson {
      virtual decimal getGpa() const;
  };
  struct IEmployee : virtual IPerson {
      virtual int getSalary() const;
  };
  struct final MagicStudent : virtual IStudent, virtual IEmployee {
      std::string getName() const override { return name_; }
  private:
      std::string name_; // ...
  };
  void printEmployee(const IEmployee&);
  void printStudent(const IStudent&);
  ```
* Их можно очень удобно смешивать в любых пропорциях.
  * Каждый метод встречается ровно один раз.
  * Все конструкторы интерфейсов тривиальны.
  * Разве что много-много вспомогательных указателей создаётся.
* Такое активно используется в Java, там на каждый чих создаётся интерфейс.
