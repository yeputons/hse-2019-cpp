#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto mid = std::partition(std::begin(a), std::end(a), [](int x) { return x % 2 == 0; });
    std::copy(std::begin(a), mid, std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
    std::copy(mid, std::end(a), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
}
