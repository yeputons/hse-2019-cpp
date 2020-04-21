#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>

namespace ns {
    struct Foo { int x; };
    const int* begin(const Foo &f) { return &f.x; }
    const int* end(const Foo &f) { return &f.x + 1; }
};

int main() {
    ns::Foo f{20};
    for (int x : f) std::cout << x << '\n';
}
