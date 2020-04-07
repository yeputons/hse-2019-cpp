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
// values = {{1, 3, 4}}
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
