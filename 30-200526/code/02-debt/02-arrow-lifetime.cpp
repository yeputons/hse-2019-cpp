#include <iostream>
#include <string>

struct Foo {
    std::string s = "hello";
    Foo() {
        std::cout << "Foo()\n";
    }
    ~Foo() {
        std::cout << "~Foo()\n";
    }

    // Or 'auto'
    auto* operator->() { return &s; }
};

struct Bar {
    Foo operator->() { return Foo{}; }
};

int main() {
    std::cout << "|" << Bar()->c_str() << "|\n";
}
