#include <iostream>
#include <type_traits>

template<typename> using void_t = void;

template<typename T>
struct MyClass {
    template<typename U = T, std::enable_if_t<std::is_same_v<U, int>>* = nullptr>
    int foo() {
        static_assert(std::is_same_v<T, int>);
        return 10;
    }
    template<typename U = T, std::enable_if_t<std::is_same_v<U, double>>* = nullptr>
    int foo() {
        return 20;
    }
    // template<typename U = T, std::void_t<decltype(&U::foo)>* = nullptr>  // &U::foo — взятие адреса члена, конструкция до C++11 без declval<>. Не работает, если `foo` перегружен.
    template<typename U = T, std::void_t<decltype(std::declval<U>().foo)>* = nullptr>
    int foo() {
        return 30;
    }
    int foo() const {
        return 40;
    }
};

struct Botva { int foo; };

int main() {
    MyClass<int> a;
    std::cout << a.foo() << "\n";  // 10

    MyClass<double> b;
    std::cout << b.foo() << "\n";  // 20

    MyClass<Botva> c;
    std::cout << c.foo() << "\n";  // 30

    MyClass<unsigned int> d;
    std::cout << d.foo() << "\n";  // 40
}
