#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using std::string;

int main() {
    assert(std::vector<int>{1}.size() == 1);
    assert(std::vector<char>{1}.size() == 1);
    assert(std::vector<string>{1}.size() == 1);

    // START SOLUTION
    assert(std::vector<int>{1}.size() != 1);
    assert(std::vector<char>{1}.size() != 1);
    #if 1
    std::vector<string>{1};
    #endif
    // END SOLUTION
    std::cout << "PASSED\n";
}
