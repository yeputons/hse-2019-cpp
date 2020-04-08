#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::vector<int> a = {1, 2, 3, 4, 3, 3, 7, 3, 9, 10};
    // Удалите из вектора все тройки. Выведите результат на экран.
    a.erase(std::remove(a.begin(), a.end(), 3), a.end());
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
