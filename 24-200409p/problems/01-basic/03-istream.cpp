#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <vector>

using namespace std;

int main() {
    stringstream s("1 2 3 4 5");
    vector<int> to(5);

    // START SOLUTION: std::istream_iterator
    TODO
    // END SOLUTION

    assert((to == vector{1, 2, 3, 4, 5}));
}
