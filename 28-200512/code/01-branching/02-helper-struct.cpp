#include <iostream>
#include <type_traits>
#include <vector>

template<typename /*T*/>
struct PrintAllHelper {};

template<typename T>
void printAll(const T &value) {
    // До C++17 приходится выбирать ветку для компиляции через специализацию структуры.
    PrintAllHelper<T>::printAll(value);
}

template<>
struct PrintAllHelper<int> {
    static void printAll(const int &value) {
        std::cout << value << "\n";
    }
};

template<typename T>
struct PrintAllHelper<std::vector<T>> {
    static void printAll(const std::vector<T> &value) {
        for (const auto &v : value) {
            ::printAll(v);  // '::' чтобы вызвался не метод, а глобальная функция
        }
    }
};

int main() {
    printAll(10);
    printAll(std::vector{1, 2, 3});
}
