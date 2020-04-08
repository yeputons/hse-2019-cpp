#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // Передвиньте чётные элементы массива `a` в начало, а нечётные — в конец.
    // Выведите чётные элементы на одной строчке, а нечётные — на следующей.
    // Разрешается вывести лишний пробел в конце каждой строки.
    auto mid = std::partition(std::begin(a), std::end(a), [](int x) { return x % 2 == 0; });
    std::copy(std::begin(a), mid, std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
    std::copy(mid, std::end(a), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
}
