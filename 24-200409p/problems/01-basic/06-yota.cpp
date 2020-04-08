#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

int main() {
    auto myIota =
        [state = 0]() mutable {
            return state++;
        };
    assert(myIota() == 0);
    assert(myIota() == 1);
    assert(myIota() == 2);
    assert(myIota() == 3);

    vector<int> to(5);
    // START SOLUTION: std::generate
    TODO
    // END SOLUTION
    assert((to == vector{4, 5, 6, 7, 8}));
}
