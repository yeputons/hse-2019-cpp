#include <cassert>
#include <memory>
#include <iostream>

using std::cout;
using std::make_shared;
using std::shared_ptr;
using std::weak_ptr;

struct Foo {
    Foo() {
        cout << "Foo()\n";
    }
    ~Foo() {
        cout << "~Foo()\n";
    }
};

int main() {
    auto a = make_shared<Foo>();

    weak_ptr<Foo> c = a;
    shared_ptr<Foo> c2 = c.lock();
    assert(a.get() == c2.get());
    a.reset();
    assert(c2);
    c2.reset();

    c2 = c.lock();
    assert(!c2);
    return 0;
}
