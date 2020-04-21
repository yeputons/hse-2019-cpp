#include <iostream>
namespace ns {
    struct Foo {
        friend void foo(Foo) { std::cout  << "x\n"; }  // (1)
    };
    void foo(Foo);  // (2)
}
int main() {
    ns::Foo f;
    foo(f);  // (1)
    ns::foo(f);  // (2)
}
