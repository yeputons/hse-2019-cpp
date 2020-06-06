#include <algorithm>
#include <iterator>
#include <iostream>

struct Foo {
    void foo() & {}
    void foo() && {}
    void foo() const& {}

    void bar() {}
    void bar() const {}
};

int main() {
    Foo f;
    f.foo();
    Foo().foo();
    return 0;
}
