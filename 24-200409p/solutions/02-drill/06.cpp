#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4, 3, 3, 7, 3, 9, 10};
    // Удалите из вектора все тройки. Выведите результат на экран.
    a.erase(remove(a.begin(), a.end(), 3), a.end());
    copy(begin(a), end(a), ostream_iterator<int>(cout, "\n"));
}
