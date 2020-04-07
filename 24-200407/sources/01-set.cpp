#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <type_traits>

using std::cout;
using std::set;
using std::pair;

int main() {
    // Базовые операции.
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

    // Итераторы.
    {
        // values == {5, 10, 15}
        auto it = values.lower_bound(11);
        // Аналогично есть upper_bound, equal_range...
        assert(*it == 15);
        --it;
        assert(*it == 10);
        // *it = 5; // Не компилируется.
    }
    {
        // Работает, но за линию.
        auto it = std::lower_bound(values.begin(), values.end(), 11);
        assert(*it == 15);
    }

    // Работает с range-based-for.
    for (const int &value : values) {
        cout << value << "\n";  // 5 10 15
    }
    for (const auto &value : values) {
        cout << value << "\n";  // 5 10 15
    }

    // Вставка.
    values.insert(7);  // {5, 7, 10, 15}
    assert((values == set{5, 7, 10, 15}));  // Двойные скобки, потому что assert - макрос, и не знает про {} и ,
    assert(values.count(7) == 1);
    assert(values.find(7) != values.end());
    assert(values.size() == 4);

    // Удаление по значению.
    assert(values.erase(7) == 1); assert(values.size() == 3);
    assert(values.erase(7) == 0); assert(values.size() == 3);

    // Удаление по итератору. std::remove_if тут не поможет, потому что надо модифицировать контейнер.
    for (auto it = values.begin(); it != values.end();) {
        if (*it % 2 == 0) {
            it = values.erase(it);
            assert(*it == 15);
        } else {
            ++it;
        }
    }

    // Вставка возвращает пару (итератор, успешна ли вставка).
    pair<set<int>::iterator, bool> insert12a = values.insert(12); 
    // values == {5, 12, 15}
    assert(*insert12a.first == 12);
    assert(insert12a.second);
    const int *addr12 = &*insert12a.first;
    // Итераторы и элементы не инвалидируются.
    values.insert(1);
    assert(values.find(12) == insert12a.first);
    assert(&*values.find(12) == addr12);

    // Повторная вставка неуспешна.
    auto insert12b = values.insert(12);
    assert(!insert12b.second);
    assert(insert12a.first == insert12b.first);
}
