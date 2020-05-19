#include <type_traits>
#include <vector>
#include <set>

/*integral_constant<T, T v> {
    static constexpr T value = v;
}*/
using std::vector;
using std::set;

template<typename, typename = void>
struct Foo : std::integral_constant<int, 10> {
};

template<typename T>
struct Foo<T, std::enable_if_t<std::is_integral_v<T>>> : std::integral_constant<int, 20> {};
//     Foo<T, void>
// is_floating_v
// is_arithmetic_v == is_integral || is_floating_v

template<typename T>
struct Foo<vector<T>, std::enable_if_t<std::is_integral_v<T>>> : std::integral_constant<int, 25> {};

template<typename T>
struct Foo<set<T>> : std::integral_constant<int, 30> {};

int main() {
    static_assert(Foo<double>::value == 10);
    static_assert(Foo<void>::value == 10);
    static_assert(Foo<int>::value == 20);
    static_assert(Foo<vector<int>>::value == 25);
    static_assert(Foo<vector<double>>::value == 10);
    static_assert(Foo<set<double>>::value == 30);
}
