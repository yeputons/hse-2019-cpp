#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

using namespace std;

int main() {
    // Заведите vector<int> и запишите в него десять случайных чисел от 5 до 15 включительно.
    // Выведите на экран.
    mt19937 gen;
    uniform_int_distribution distrib(5, 15);
    vector<int> a(10);
    generate(a.begin(), a.end(), bind(distrib, gen));
    copy(begin(a), end(a), ostream_iterator<int>(cout, "\n"));
}
