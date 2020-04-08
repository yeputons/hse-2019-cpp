#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::mt19937 gen;
    std::uniform_int_distribution distrib(5, 15);
    std::vector<int> a;
    std::generate_n(std::back_insert_iterator(a), 10, std::bind(distrib, gen));
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
