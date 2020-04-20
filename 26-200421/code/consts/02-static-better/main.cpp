#include "foo.h"
#include <iostream>

int main() {
    std::cout << Foo::N << '\n';
    std::cout << Foo::Name[0] << '\n';
    std::cout << &Foo::N << '\n';
    std::cout << &Foo::Name[0] << '\n';
    foo();
}
