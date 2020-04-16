#include <iostream>

using namespace std;

int foo(int *a, long *b) {
    *a = 1;
    *b = 0;
    return *a;
}

int main() {
    int a = 10;
    long b = 20;
    cout << foo(&a, &b) << "\n"; // OK
    cout << foo(&a, reinterpret_cast<long*>(&a)) << "\n";  // UB
}
