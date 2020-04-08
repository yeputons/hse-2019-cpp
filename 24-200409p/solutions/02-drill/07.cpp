#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4, 3, 3, 7, 1, 3, 9, 10};
    // Удалите из вектора все дубликаты. Разрешается переставлять элементы. Выведите результат на экран.
    sort(a.begin(), a.end());
    a.erase(unique(a.begin(), a.end()), a.end());
    copy(begin(a), end(a), ostream_iterator<int>(cout, "\n"));
}
