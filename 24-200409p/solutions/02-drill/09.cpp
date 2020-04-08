#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

using namespace std;

int main() {
    vector<int> a = {2, 3, 3, 1};
    // Выведите сумму квадратов элементов `a`.
    cout << accumulate(begin(a), end(a), 0, [](int acc, int x) { return acc + x * x; }) << "\n";
}
