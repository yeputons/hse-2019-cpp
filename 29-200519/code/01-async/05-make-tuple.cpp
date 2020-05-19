#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

int main() {
    int a = 10;
    int b = 20;
    std::tuple<int, int&> t(a, std::ref(b));
    t = std::make_tuple(40, 50);
    std::cout << a << " " << b << "\n";
}
