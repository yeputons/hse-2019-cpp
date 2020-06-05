#include <iostream>
#include <memory>

struct Foo {
    template<typename T, typename = decltype(std::declval<T>().foo())>
    Foo(T) {
        std::cout << "1\n";
    }

    template<typename T>
    explicit Foo(T) {
        std::cout << "2\n";
    }
};

struct Bar {
    void foo() {}
};

int main() {
    Foo f1 = Bar();  // 2 is not available because it's explicit
    Foo f2(10);      // 1 is not available because of SFINAE error
    return 0;
}
