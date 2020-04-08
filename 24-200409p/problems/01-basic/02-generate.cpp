#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

// START SOLUTION: declare your own `Generator` functor
TODO
// END SOLUTION

int main() {
    vector<int> to(5);
    generate(to.begin(), to.end(), Generator(10));
    assert((to == vector{10, 11, 12, 13, 14}));
}
