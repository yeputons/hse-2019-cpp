#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

int main() {
    vector<int> vec = {1, 2, 3, 4, 6, 5};

    // START SOLUTION: std::remove_if
    vec.erase(
        remove_if(vec.begin(), vec.end(), [&](int x) { return x % 2 == 0; }),
        vec.end()
    );
    // END SOLUTION

    assert((vec == vector{1, 3, 5}));
}
