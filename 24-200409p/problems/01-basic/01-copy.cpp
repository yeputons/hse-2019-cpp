#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

int main() {
    vector<int> from = {1, 2, 3, 4, 5};
    vector<int> to(5);

    // START SOLUTION: std::copy
    TODO
    // END SOLUTION

    assert((from == vector{1, 2, 3, 4, 5}));
    assert((to == vector{1, 2, 3, 4, 5}));
}
