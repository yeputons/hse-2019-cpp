#include <type_traits>

template<typename> constexpr bool false_ = false;

template<typename T>
auto foo(const T &) {
    if constexpr (std::is_same_v<char*, T>) {
        // static_assert(false);  // Не работает.
        // Альтерантива: проверить условие ещё раз:
        static_assert(!std::is_same_v<char*, T>);
    }

    if constexpr (constexpr bool x = std::is_same_v<char*, T>; x) {
        static_assert(!x);
    }

    // Но тогда можно было бы и сразу написать static_assert(!условие), без if constexpr.

    if constexpr (std::is_same_v<int, T>) {
        return 10.0;
    } else if constexpr (std::is_same_v<double, T>) {
        return 10;
    } else {
        // static_assert(false);  // Ошибка компиляции всегда :(
        static_assert(false_<T>);  // Надо обмануть компилятор.
    }
}

int main() {
    static_assert(2 * 2 == 4);
    static_assert(2 * 2 == 4, "Ok");
    // static_assert(2 * 2 == 5, "Oops");

    foo(10);
    foo(10.0);
}
