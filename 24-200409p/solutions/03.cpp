#include <algorithm>
#include <iostream>
#include <iterator>

int main() {
    int a[5] = {2, -4, 3, 5, 1};
    std::for_each(std::begin(a), std::end(a), [](int &x) { x += 1; });
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
