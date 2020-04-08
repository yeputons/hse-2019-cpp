#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <vector>

using namespace std;

int main() {
    stringstream s;
    vector<int> from = {1, 2, 3, 4, 5};

    // START SOLUTION: std::ostream_iterator
    copy(from.begin(), from.end(), ostream_iterator<int>(s, "\n"));
    // END SOLUTION

    assert(s.str() == "1\n2\n3\n4\n5\n");
}
