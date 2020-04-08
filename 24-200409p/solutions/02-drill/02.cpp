#include <algorithm>
#include <cassert>

int main() {
    int a[] = {1, 2, 6, 7, 9, 4};
    int c[] = {36, 49, 81};
    auto answer = std::search(std::begin(a), std::end(a), std::begin(c), std::end(c), [](int haystack, int needle) { return haystack * haystack == needle; });
    assert(answer == a + 2);
}
