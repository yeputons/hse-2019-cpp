#include <iostream>
#include <tuple>

void bar(int a, int b) {
    std::cout << a << ", " << b << "\n";
}

template<typename ...Args>
void foo(Args ...args) {
    std::cout << sizeof...(args) << "\n";
    std::tuple<Args...> t(args...);
    bar(2 + args...);
}

int main() {
    foo(10, 20);
}
