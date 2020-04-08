#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    int a[] = {1, 2, 3, 4, 5};
    int b[] = {3, 1, 5, 2, 5};
    std::cout << std::inner_product(
        std::begin(a), std::end(a),
        std::begin(b),
        0,
        std::plus<int>(),
        std::greater<int>()
    ) << "\n";
}
