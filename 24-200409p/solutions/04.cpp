#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::mt19937 gen;
    std::uniform_int_distribution distrib(5, 15);
    std::vector<int> a(10);
    std::generate(a.begin(), a.end(), std::bind(distrib, gen));
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
