#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int main() {
    std::vector<int> a = {1, 2, 3, 4, 3, 3, 7, 1, 3, 9, 10};
    // Удалите из вектора все дубликаты. Разрешается переставлять элементы. Выведите результат на экран.
    sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());
    std::copy(std::begin(a), std::end(a), std::ostream_iterator<int>(std::cout, "\n"));
}
