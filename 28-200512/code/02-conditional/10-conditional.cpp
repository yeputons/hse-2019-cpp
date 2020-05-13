#include <type_traits>

// "До"
template<typename T>
auto foo(T x) {
    if constexpr (std::is_same_v<T, int>) {
        return static_cast<double>(x);
    } else if constexpr (std::is_same_v<T, long long>) {
        return static_cast<long double>(x);
    }
}

// Кусок стандартной библиотеки (std::conditional).
template<bool Cond, typename T, typename F>
struct conditional {
    using type = T;
};
template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template<bool Cond, typename T, typename F>
using conditional_t = typename conditional<Cond, T, F>::type;

// "После"
template<typename T>
auto bar(T x) {
    return static_cast<
        conditional_t<
            std::is_same_v<T, int>,
            double,
            long double
        >
    >(x);
}

int main() {
    foo(10);
    foo(11LL);

    bar(10);
    bar(11LL);
}
