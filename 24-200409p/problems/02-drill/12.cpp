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
    set<pair<int, int>> s;
    // END SOLUTION

    s.emplace(1, 1);
    s.emplace(1, 2);
    s.emplace(2, 1);
    s.emplace(2, 2);

    assert((vector(s.begin(), s.end()) == vector<pair<int, int>>{{1, 2}, {1, 1}}));
}
