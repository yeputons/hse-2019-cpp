#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::vector<int> a = {2, 3, 3, 1};
    // Выведите все 4!/2! = 12 перестановок мультимножества `a`, каждую на отдельной строке.
    // Разрешается выводить лишний пробел в конце строк.
    sort(a.begin(), a.end());
    do {
        std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
    } while (next_permutation(a.begin(), a.end()));
}
