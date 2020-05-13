#include <cassert>
#include <type_traits>

template<typename T>
struct Foo {
    // Это уже при помощи if constexpr не проэмулировать.
    std::conditional_t<
        std::is_same_v<T, int>,
        double,
        T
    > value;
};

int main() {
    Foo<int> x{10.5};
    assert(x.value == 10.5);

    Foo<char> y = {10};
    assert(y.value == 10);
    y.value += 0.5;
    assert(y.value == 10);
}
