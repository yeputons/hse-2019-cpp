#include <cassert>
#include <set>
#include <utility>
#include <vector>

using std::set;
using std::pair;
using std::vector;

int main() {
    set<vector<int>> values = {
        {1, 2, 3},
        {1, 3, 4},
    };
    // Можно конструировать элементы внутри
    values.emplace(2, 4);  // {4, 4}

    // Можно давать подсказки insert: перед каким элементом вставить.
    // Тогда он работает быстрее O(log), если мы не ошиблись с подсказкой.
    auto hint = values.lower_bound({1, 3});
    assert((*hint == vector{1, 3, 4}));
    values.insert(hint, {1, 3, 3});
    // Есть emplace_hint.    

    // Начиная с C++17 можно перетаскивать элементы между контейнерами.
    // Можно избежать перевыделения памяти.
    set<vector<int>>::node_type node = values.extract(hint);
    set<vector<int>> values2;
    values2.insert(std::move(node));
    assert((values == set{vector{1, 2, 3}, {1, 3, 3}, {4, 4}}));
    assert((values2 == set{vector{1, 3, 4}})); 
}
