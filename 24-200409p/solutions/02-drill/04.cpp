#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    // Заведите vector<int> и запишите в него десять случайных чисел от 5 до 15 включительно.
    // Выведите на экран.
    std::mt19937 gen;
    std::uniform_int_distribution distrib(5, 15);
    std::vector<int> a(10);
    std::generate(a.begin(), a.end(), std::bind(distrib, gen));
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
