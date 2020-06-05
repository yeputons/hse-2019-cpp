#include <iostream>
#include <vector>

using namespace std;

void bar(const int &) {
    std::cout << "1\n";
}

void bar(int &&) {
    std::cout << "2\n";
}

template<typename... T>
void foo(T &&...x) {
    bar(std::forward<T>(x)...);
}

int main() {
    int x;
    foo(x);   // 1
    foo(10);  // 2
    return 0;
}
