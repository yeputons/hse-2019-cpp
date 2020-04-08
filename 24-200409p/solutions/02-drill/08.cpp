#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

using namespace std;

int main() {
    vector<int> a = {2, 3, 3, 1};
    // Выведите все 4!/2! = 12 перестановок мультимножества `a`, каждую на отдельной строке.
    // Разрешается выводить лишний пробел в конце строк.
    sort(a.begin(), a.end());
    do {
        copy(begin(a), end(a), ostream_iterator<int>(cout, " "));
        cout << "\n";
    } while (next_permutation(a.begin(), a.end()));
}
