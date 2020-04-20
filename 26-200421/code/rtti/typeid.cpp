#include <cassert>
#include <typeinfo>

struct Base { virtual ~Base(); int x; };
struct Derived : Base { virtual ~Derived(); int y; };

struct A { int x; };
struct B : A { int y; };
A foo();  // Никогда не вызывается.

int main() {
    const std::type_info &t1 = typeid(Base);
    const std::type_info &t2 = typeid(Base*);
    assert(typeid(Base) == typeid(const Base&));

    assert(typeid(foo()) == typeid(A));
    B b;
    const A &bref = b;
    assert(typeid(bref) == typeid(A));
}
