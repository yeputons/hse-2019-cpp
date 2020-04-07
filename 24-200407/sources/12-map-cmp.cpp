#include <cassert>
#include <set>
#include <vector>

using std::set;
using std::vector;

struct CompareK {
    int k;
    CompareK(int k_) : k(k_) {}
    bool operator()(int a, int b) const {
        return a * k < b * k;
    }
};

bool compare(int a, int b) { return a > b; }

int main() {
    {
        set<int, CompareK> values(CompareK(-1));
        values.insert(1);
        values.insert(2);
        assert((vector(values.begin(), values.end()) == vector{2, 1}));
    }

    {
        set<int, bool(*)(int, int)> values(compare);
        values.insert(2);
        values.insert(1);
        assert((vector(values.begin(), values.end()) == vector{2, 1}));
    }

    {
        auto compare = [](int a, int b) { return a > b; };
        set<int, decltype(compare)> values(compare);
        values.insert(2);
        values.insert(1);
        assert((vector(values.begin(), values.end()) == vector{2, 1}));
    }
}
