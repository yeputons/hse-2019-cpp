#include <iostream>
#include <type_traits>
#include <vector>

template<typename T>
auto printAll(const T &value) {
    if constexpr (std::is_same_v<T, int>) {
        std::cout << value << "\n";
        return 10;  // И даже автовывод возвращаемого типа работает: в одной ветке int, в другой void.
    } else if constexpr (false) {  // Цепочка else-if constexpr работает.
        value.doSomethingWeird();  // Никогда не компилируется.
    } else {
        for (const auto &v : value) {
            printAll(v);
        }
    }

}

void doNotDoIt() {
    // Не надо мешать if и constexpr if. Сложно понимать, что когда скомпилируется.
    if constexpr (2 * 2 == 5) {
    } else if (2 * 2 == 4) {
    } else if constexpr (2 * 2 == 6) {
    }
}

int main() {
    int x = printAll(10);
    printAll(std::vector{1, 2, 3});
}
