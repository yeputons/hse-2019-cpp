#include <algorithm>
#include <cassert>
#include <set>
#include <utility>
#include <vector>

using namespace std;

int main() {
    // Пары внутри set<> должны быть отсортированы по полю `.second` по убыванию.
    // Поле `.first` полностью игнорируется.

    // START SOLUTION
    auto comparator = [](const pair<int, int> &a, const pair<int, int> &b) { return a.second > b.second; };
    set<pair<int, int>, decltype(comparator)> s(comparator);
    // END SOLUTION

    s.emplace(1, 1);
    s.emplace(1, 2);
    s.emplace(2, 1);
    s.emplace(2, 2);

    assert((vector(s.begin(), s.end()) == vector<pair<int, int>>{{1, 2}, {1, 1}}));
}
