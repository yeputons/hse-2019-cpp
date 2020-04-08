#include <algorithm>
#include <cassert>
#include <set>

using namespace std;

void erase_all_even(multiset<int> &s);

int main() {
    {
        multiset<int> m = {1, 2, 3, 3, 4, 5};
        erase_all_even(m);
        assert((m == multiset<int>{1, 3, 3, 5}));
    }

    // START SOLUTION: part 1, find a test case which crashes erase_all_even() below.
    TODO
    // END SOLUTION
}

void erase_all_even(multiset<int> &s) {
    // START SOLUTION: part 2, fix the bug.
    for (int x : s) {
        if (x % 2 == 0) {
            s.erase(x);
        }
    }
    // END SOLUTION
}
