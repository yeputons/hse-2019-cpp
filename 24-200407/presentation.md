## `set`
* "Ассоциативный контейнер" из containers library.
* Хранит отсортированное множество элементов без повторений.

```c++
set<int> values = {10, 5, 5, 15};  // Сам отсортирует и уберёт повторы.
{
    // Если инициализируем списком, то можно опустить знак `=`,
    // это называется uniform initialization, мы ещё вернёмся на следующей лекции.
    set<int> values2{10, 5, 15};
    // С C++17 работает автовывод типа (как и с любым контейнером).
    set values3 = {10, 5, 15};
    set values4 = values;
}
assert(values.count(7) == 0);
assert(values.find(7) == values.end());
assert(values.count(5) == 0);
assert(*values.find(5) == 5);
```

* Нет доступа по индексу.
* Все операции с элементами работают за `O(log n)` сравнений.
* Каждый элемент создаётся один раз и никогда не двигается (внутри — дерево).

---
## Итераторы `set`
```c++
// values == {5, 10, 15}
auto it = values.lower_bound(11); // Аналогично есть upper_bound, equal_range...
assert(*it == 15);
--it;  // Bidirectional
assert(*it == 10);
// *it = 5; // Не компилируется.
```
Работает, но за линию:
```c++
auto it = std::lower_bound(values.begin(), values.end(), 11);
assert(*it == 15);
```
Работает с range-based-for:
```c++
for (const int &value : values) {
    cout << value << "\n";  // 5 10 15
}
for (const auto &value : values) {
    cout << value << "\n";  // 5 10 15
}
```

---
## Вставка и сравнения
```c++
values.insert(7);
assert((values == set{5, 7, 10, 15}));
assert(values.count(7) == 1);
assert(values.find(7) != values.end());
assert(values.size() == 4);
```

* Nit: двойные скобки в `assert` нужны из-за запятых

Вставка возвращает пару `(итератор, успешна ли вставка)`:
```c++
pair<set<int>::iterator, bool> insert12a = values.insert(12); 
// values == {5, 12, 15}
assert(*insert12a.first == 12);
assert(insert12a.second);

auto insert12b = values.insert(12);
assert(!insert12b.second);
assert(insert12a.first == insert12b.first);
```

Итераторы и элементы не инвалидируются.

---
## Удаление по значению
```c++
assert(values.erase(7) == 1); assert(values.size() == 3);
assert(values.erase(7) == 0); assert(values.size() == 3);
```

## Удаление по итератору
* Хотим удалить все чётные числа.
* `std::remove_if` не поможет: нельзя переставлять элементы.

```c++
// values == {5, 7, 10, 15}
//                  ^------- удаляем
//                       ^-- erase возвращает следующий элемент
for (auto it = values.begin(); it != values.end();) {
    if (*it % 2 == 0) {
*       it = values.erase(it);
        assert(*it == 15);
    } else {
        ++it;
    }
}
```

---
## `multiset`
Аналогичен `set`, хранит несколько копий:
```c++
// Порядок я не знаю, но могу представить реализацию,
// где он в процессе меняется.
multiset<int> values = {10, 5, 5, 5, 5, 5, 5, 15};
assert(values.count(7) == 0);
assert(values.count(5) == 6); // Работает за O(log n + answer)
assert(values.find(7) == values.end());
assert(values.find(5) != values.end());  // Может вернуть любую копию.
```

Вставка всегда успешна:
```c++
multiset<int>::iterator new5 = values.insert(5);
assert(*new5 == 5);
```

---
## Удаление из `multiset`
Удаление по итератору удаляет один элемент:
```c++
multiset<int>::iterator it = values.erase(values.lower_bound(5));
// {5, 5, 5, 5, 5, 5, 10, 15}
//  ^------- удалили
//     ^---- erase вернул
assert(*it == 5);  // Важно, что был именно lower_bound.
assert(values.size() == 8);
```

Удаление по значению удаляет все элементы за `O(log n + count)`:
```c++
assert(values.erase(5) == 6);
assert(values.size() == 2);
```

---
## Прочее про `set` и `multiset`
```c++
set<vector<int>> values = {{1, 2, 3}, {1, 3, 4}};
// Можно вызвать конструктор элемента напрямую: emplace вместо insert.
values.emplace(2, 4);  // Добавит vector<int>(2, 4) == {4, 4}
```

Можно давать подсказки insert: перед каким элементом вставить.
Тогда он работает быстрее `O(log n)`, если мы не ошиблись с подсказкой.
```c++
auto hint = values.lower_bound({1, 3, 3});
assert((*hint == vector{1, 3, 4}));
values.insert(hint, {1, 3, 3});
// Есть emplace_hint(hint, 2, 4);
```

Начиная с C++17 можно перетаскивать элементы между контейнерами.
Можно избежать перевыделения памяти.
```c++
set<vector<int>>::node_type node = values.extract(hint);
set<vector<int>> values2;
values2.insert(std::move(node));  // std::move обязателен
// values = {{1, 2, 3}, {1, 3, 3}, {4, 4}}
// values2 = {{1, 3, 4}}
```

---
## `map`
Отображение "ключ" - "значение", ключи отсортированы, интерфейс аналогичен `set`.
```c++
map<string, int> m = {
    {"key", 10},
    {"foo", 20}
};
assert(m.size() == 2);
assert(m.count("key") == 1);
assert(m["key"] == 10);
```

Осторожно с `operator[]`
```c++
if (m["bar"]) { .... }
// {{"bar", 0}, {"foo", 20}, {"key", 10}}
```
* `operator[]` всегда должен вернуть ссылку на элемент.
* Следствие: требуется конструктор по умолчанию у значения.
* Следствие: есть только у неконстантного `map`.

```c++
assert(m["wtf"] == 0);  // Работает!
```

---
## Итераторы `map`
Выглядит как контейнер `std::pair<const Key, Value>`
* `map<string, int>::value_type == std::pair<const string, int>`
* `map<string, int>::key_type == string`
* `map<string, int>::mapped_type == int`

```c++
// {{"bar", 0}, {"foo", 20}, {"key", 10}, {"wtf", 0}}
map<string, int>::iterator it = m.find("foo");
assert(it == std::next(m.begin()));
assert(it->first == "foo");
assert(it->second == 20);
// it->first += "x";  // Нельзя менять ключ.
it->second += 3;  // Значение менять можно.
```

Всё ещё можно делать `emplace` для `value_type`:
```c++
class map { .. pair<iterator, bool> emplace(const std::string, int); .. }
....
assert(m.emplace("super", 33).second);
assert(m["super"] == 33);
```

---
## Компараторы
`set` работает с любыми компараторами:

```c++
template<typename T, typename Cmp = std::less<T>>
class set {
    Cmp cmp;
    set() : cmp() {}
    set(Cmp cmp_) : cmp(cmp_) {}
    ....
        if (cmp(node->key, key_to_find)) { .... } else { .... }
    ....
};
template<typename T>
class less {
     bool operator()(const T &a, const T &b) const/*!!!*/ {
        return a < b;  // Должен ли a быть строго раньше b?
    }
}
```

---
## Свои функторы
Объявляем функтор:
```c++
struct CompareK {
    int k;
    CompareK(int k_) : k(k_) {}
    bool operator()(int a, int b) const {
        return a * k < b * k;
    }
};
```
Используем:
```c++
set<int, CompareK> values(CompareK(-1));
values.insert(1);
values.insert(2);
// {2, 1}
```

---
## Лямбды и функции
Надо всегда явно указать тип компаратора.
```c++
void compare(int a, int b) { return a > b; }
....
set<int, void(*)(int, int)> values(compare);
values.insert(2);
values.insert(1);
// {2, 1}
```

Лямбды надо объявить заранее и использовать `decltype`:
```c++
auto compare = [](int a, int b) { return a > b; };
set<int, decltype(compare)> values(compare);
values.insert(2);
values.insert(1);
```

Для функций `decltype` тоже отработает.

---
## Неупорядоченные контейнеры
Можно дописать `unordered_`, будет хэш-таблица со списками.

```c++
unordered_set<int> values = {1, 2, 3, 4, 5};
for (int v : values) cout << v << "\n";  // Порядок неизвестен.
```

* Работает за `O(1)` амортизированно, но за `O(n)` в худшем случае.
* Динамически меняет свой размер и количество корзин.
* Обычно быстрее `set`.
* Скорее всего, для простых типов медленнее ручной реализации.
* Если очень надо, нормально делать свою хэш-таблицу под конкретные условия.
* Есть варианты с `multi` и с `map`

Лучше использовать `unordered_`-версию.

---
## Свой хэш
* Объект должен сравниваться на `==`.
* Хэширующая функция возвращает `std::size_t` (хоть `return 0`, могут быть коллизии).

```c++
struct point {
    int x = 0, y = 0;
    point() {}
    point(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const point &other) const {
        return x == other.x && y == other.y;
    }
};
struct PointHasher {
    size_t operator()(const point &p) const {
        return std::hash<int>()(p.x) * 239017 + std::hash<int>()(p.y);
    }
};
....
unordered_set<point, PointHasher/*, std::equal_to<point>*/> points;
points.emplace(10, 20);  // points.size() == 1
points.emplace(20, 10);  // points.size() == 2
points.emplace(10, 20);  // points.size() == 2
```

---
## `stack`, `queue`
Обёртки над `deque`, у которых убрали методы.

```c++
template<typename T, typename Container = std::deque<T>>
struct stack {
    Container c;
    void push(const T &value) { c.push_back(value); }
    void push(T &&value) { c.push_back(std::move(value)); }
    void pop() { c.pop_back(); }
    T& top() { return c.back(); }
    const T& top() const { return c.back(); }
};
```
```с++
template<typename T, typename Container = std::deque<T>>
struct queue {
    Container c;
    ....
    void pop() { c.pop_front(); }
    T& front() { return c.front(); }
    const T& front() const { return c.front(); }
    .... back() ....
};
```

---
## `priority_queue`
"Очередь с приоритетом", двоичная куча в каком-то контейнере:
```c++
template<typename T, typename Container = std::vector<T>, typename Compare = ....>
struct priority_queue {
    Container c;
    ....
    const T& top() { return c[0]; }
    void push(const T &value) {
        c.push_back(value);
        std::push_heap(....);
    }
    void push(T &&value);
    void pop() {
        std::pop_heap(....);
        c.pop_back();
    }
};
```

* Итераторов нет, удалять можно только верхний элемент.
* Функции `std::push_heap`/`std::pop_heap` можно вызывать самостоятельно.
* Работает ощутимо быстрее `set<T>`.

---
## `bitset`
Последовательность из `N` фиксированных бит:
```c++
bitset<10> bs(   "0000010011");
bs.set(2);     // 0000010111
assert(bs.test(2));
assert(bs[2]);
bs.reset(2);   // 0000010011
bs.flip();     // 1111101100
assert(bs.count() == 7);
bs ^= bitset<10>("1100000001");
               // 0011101101
assert(bs.count() == 6);
```

* Конвертируется в строчки, выводится на экран...
* Операции с битами медленнее `vector<char>`.
* Массовые операции быстрее `vector<char>` в десяток раз.
* Полезно для ускорения динамического программирования.

---
# Хранение объектов
Смотри [GotW 89](https://herbsutter.com/2013/05/29/gotw-89-solution-smart-pointers/) и
[GotW 91](https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/).

## Простые переменные и поля

* Если вы всегда владеете объектом единолично — по значению, идеальный вариант:
  ```c++
  struct Dsu {
      vector<int> parent;
      Dsu(int n) : parent(n, -1) {}
  };
  ```
* Если надо иногда не хранить объект и нет рекурсии — `std::optional` (C++17, +1 байт, нет дополнительных выделений памяти):
  ```c++
  struct Config {
      optional<string> outputFileName;
  };
  ```

---
## Умные указатели: `unique_ptr`
* Если есть рекурсия или надо передавать владение объектом, но владелец всегда один — `std::unique_ptr`
  ```c++
  struct SearchTreeNode {
      std::unique_ptr<SearchTreeNode> left, right;
      int key; string value;
  };
  ```
* Если надо явно передать функции владение объектом, который не хочется копировать/перемещать
  ```c++
  void passToAnotherProcessAndForget(std::unique_ptr<SomeData> iOwnItNow);
  ```
* Если надо вернуть владение из функции:
  ```c++
  std::unique_ptr<SomeData> readFromAnotherProcess();
  ```

---
## Про `unique_ptr`
* Используется часто.
* Можно указать свой функтор удаления (вроде `fclose`), но лучше полностью написать RAII-обёртку (вроде `istream`/`ostream`).
* Можно узнать значение указателя для совместимости:
  ```c++
  {
      std::unique_ptr<SomeData> ptr = ....;
      SomeData *ptr2 = ptr.get();
  }  // delete;
  ```
* Можно вытащить владение указателя для совместимости:
  ```c++
  {
      std::unique_ptr<SomeData> ptr = ....;
      SomeData *ptr2 = ptr.release();
  }  // утечка
  ```

---
## Умные указатели: `shared_ptr`
* Владельцев несколько и никак не сделать одного:
  ```c++
  class Window1 {
      std::shared_ptr<SuperImportantData> data;
  };
  class Window2 {
      std::shared_ptr<SuperImportantData> data;
  };
  ```
* Выделит в куче счётчик активных ссылок.
  Удалит данные, когда тот упадёт до нуля.
* Сильно дороже `unique_ptr` из-за счётчика и многопоточности.
* Если создать второй `shared_ptr`, будет новый счётчик:
  ```c++
  {
      shared_ptr<Data> x = ....;
      shared_ptr<Data> y = x.get();
  }  // `x`, `y` вызовут `delete`
  ```

---
## Умные указатели: `weak_ptr`
* `shared_ptr` не работает с циклами:
  ```c++
  class SuperImportantData {
      std::vector<std::shared_ptr<SuperImportantData>> children;
      std::shared_ptr<SuperImportantData> parent;  // Упс, цикл.
  };
  ```
* Для разрыва циклов можно делать ссылки наверх через `std::weak_ptr`:
  ```c++
  class SuperImportantData {
      std::vector<std::shared_ptr<SuperImportantData>> children;
      std::weak_ptr<SuperImportantData> parent;
  };
  ```
* Чтобы использовать `weak_ptr`, надо его преобразовать в `shared_ptr`
  (потому что многопоточность).
* `weak_ptr` автоматически обнулится при удалении родителя.

`shared_ptr`/`weak_ptr` используется очень редко, обычно владелец один.

---
## Чистые указатели
* Совместимость с Си (лучше сразу обернуть в RAII или умный указатель).
* Ссылаемся на объект, который точно нас переживёт:
  ```
  struct SearchTreeNode {
      std::unique_ptr<SearchTreeNode> left, right;
      SearchTreeNode *parent;
      int key; string value;
  };
  ```
* Пишем свой умный указатель

---
## Создание умных указателей
Всегда используйте `make_unique`, не пишите `new`:
```c++
foo(unique_ptr<Foo>(new Foo), unique_ptr<Bar>(new Bar));
```
До C++17 компилятор мог сначала выполнить все `new`, а потом все конструкторы.
Это утечка, если один из `new` бросал.

Лучше так:
```c++
foo(make_unique<Foo>(), make_unique<Bar>());
```
.
## Наследование
```c++
unique_ptr<Derived> d = ....;
unique_ptr<Base> b = d;
```

---
## Алгоритмы
Лучше всегда использовать алгоритмы или range-based-for.

До:
```c++
vector<int> values;
for (size_t i = 0; i < values.size(); ++i) {
    if (values[i] % 2 == 0) {
        values.erase(values.begin() + i);
        --i;
    }
}
```

После (меньше шансов забыть индексы или что угодно):
```c++
values.erase(std::remove_if(values.begin(), values.end(), [](int v) {
    return v % 2 == 0;
}), values.end());
```

А ещё в C++20 появились Ranges.

---
## Многопоточность
Есть те же самые mutex, conditional variables...

```c++
std::atomic<int> atomicInt;
std::unordered_set<int> values;
std::mutex m;
void add(int value) {
    ++atomicInt;  // Операции гарантированно атомарны.
    std::lock_guard g(m);  // Взяли mutex и держим до деструктора.
    values.insert(value);
}
```

Осторожно: помните про имя у `lock_guard`: `std::lock_guard(m);`

## Прочее
* `std::chrono`
* `std::filesystem`
* `std::random`
