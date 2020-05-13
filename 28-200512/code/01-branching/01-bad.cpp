#include <iostream>
#include <type_traits>
#include <vector>

template<typename T>
void printAll(const T &value) {
    // Сейчас компилируется обе ветки => ничего не работает.
    // Несмотря на то, что у нас тут константа времени компиляции.
    if (std::is_same_v<T, int>) {
        std::cout << value << "\n";
        return 10;
    } else {
        for (const auto &v : value) {
            printAll(v);
        }
    }

}

int main() {
    printAll(10);
    printAll(std::vector{1, 2, 3});
}
