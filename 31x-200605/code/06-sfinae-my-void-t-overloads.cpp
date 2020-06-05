#include <iostream>
#include <memory>
#include <type_traits>

// NOT FOR EXAM

template<typename...>
struct my_void { using type = void; };

template<typename... Ts>
using my_void_t = typename my_void<Ts...>::type;  // Not std::my_void_t so it's not obviously always 'void' and overloads below are distinct.

struct Foo {
    template<typename T, my_void_t<decltype(std::declval<T>().foo())>* = nullptr>
    Foo(T) {
        std::cout << "1\n";
    }

    template<typename T, my_void_t<decltype(std::declval<T>().bar())>* = nullptr>
    Foo(T) {
        std::cout << "2\n";
    }
};

struct Bar {
    int x;
    int& foo() { return x; }  // decltype(foo()) is now int&, cannot have int&*
};

struct Baz {
    void bar() {}
};

int main() {
    Foo f1 = Bar();  // (1)
    Foo f2 = Baz();  // (2)
    return 0;
}
