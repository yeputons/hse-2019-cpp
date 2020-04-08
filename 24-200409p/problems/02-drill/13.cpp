#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

int main() {
    // Пары внутри unordered_set<> равны тогда и только тогда, когда равны их поля `.second`.

    // START SOLUTION
    unordered_set<pair<int, int>> s;
    // END SOLUTION

    s.emplace(1, 1);
    s.emplace(1, 2);
    s.emplace(2, 1);
    s.emplace(2, 2);

    vector v(s.begin(), s.end());
    assert((vector(s.begin(), s.end()) == vector<pair<int, int>>{{1, 2}, {1, 1}}));
}
