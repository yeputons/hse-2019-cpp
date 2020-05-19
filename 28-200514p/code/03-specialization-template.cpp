#include <type_traits>
#include <vector>
#include <set>

/*integral_constant<T, T v> {
    static constexpr T value = v;
}*/
using std::vector;
using std::set;

template<template<typename> typename, typename = void>
struct Foo : std::integral_constant<int, 10> {
};

template<>
struct Foo<vector> : std::integral_constant<int, 25> {};

template<>
struct Foo<set> : std::integral_constant<int, 30> {};

int main() {
/*    static_assert(Foo<double>::value == 10);
    static_assert(Foo<void>::value == 10);
    static_assert(Foo<int>::value == 20);*/
    static_assert(Foo<vector>::value == 25);
    static_assert(Foo<set>::value == 30);
}
