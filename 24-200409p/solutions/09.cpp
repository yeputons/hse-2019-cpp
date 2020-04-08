#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::vector<int> a = {2, 3, 3, 1};
    std::cout << std::accumulate(std::begin(a), std::end(a), 0, [](int acc, int x) { return acc + x * x; }) << "\n";
}
