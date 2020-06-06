#include <algorithm>
#include <deque>
#include <vector>
#include <stdio.h>

void printInts(const int *arr, int count) {
    for (int i = 0; i < count; i++)
        printf("%d\n", arr[i]);
}

int main() {
    std::vector<int> vec = {10, 20, 30};
    printInts(vec.data(), vec.size());  // OK
    printInts(&vec[0], vec.size());     // OK

    std::deque<int> dq = {10, 20, 30};
    // dq.push_front(-10);              // Uncomment to see UB.
    printInts(&dq[0], dq.size());       // Always UB.
    return 0;
}
