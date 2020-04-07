#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <utility>
#include <type_traits>

using std::cout;
using std::multiset;
using std::pair;

int main() {
    // multiset абсолютно аналогичен set, хранит несколько копий.
    // Порядок я не знаю, но могу представить реализацию, где он в процессе меняется.
    multiset<int> values = {10, 5, 5, 5, 5, 5, 5, 15};
    assert(values.count(7) == 0);
    assert(values.count(5) == 6); // Работает за O(log n + answer)!
    assert(values.find(7) == values.end());
    assert(values.find(5) != values.end());  // Может вернуть любую копию.

    // Вставка всегда успешна.
    multiset<int>::iterator new5 = values.insert(5);
    assert(*new5 == 5);
    
    // Удаление по итератору удаляет один элемент.
    {
        multiset<int>::iterator it = values.erase(values.lower_bound(5));
        assert(*it == 5);  // Важно, что был именно lower_bound.
        assert(values.size() == 8);
    }

    // Удаление по значению удаляет все элементы за O(log n + answer).
    assert(values.erase(5) == 6);
    assert(values.size() == 2);
}
