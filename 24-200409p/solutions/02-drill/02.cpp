#include <algorithm>
#include <cassert>

int main() {
    int a[] = {1, 6, 2, 6, 7, 9, 4};
    int c[] = {36, 49, 81};
    // Найдите итератор массива `a`, начиная с которого входит подмассив, дающий при почленном возведении в квадрат массив `c`.
    auto answer = std::search(std::begin(a), std::end(a), std::begin(c), std::end(c), [](int haystack, int needle) { return haystack * haystack == needle; });
    assert(answer == a + 3);
}
