#include <iostream>
using std::cout;

namespace ns1 {
    struct Foo {};
    namespace Foo_adl { void func(Foo) { cout << "(1)\n"; } }
    using Foo_adl::func;
}
namespace ns2 {
    struct Bar {};
    namespace Bar_adl { void func(Bar) { cout << "(2)\n"; } }
    using namespace Bar_adl;
}

void func(ns1::Foo) { cout << "(3)\n"; }
void func(ns2::Bar) { cout << "(4)\n"; }

int main() {
    ns1::Foo f; ns2::Bar b;
    ns1::func(f);  // (1), qualified lookup
    ns2::func(b);  // (2), qualified lookup
    ::func(f);     // (3), qualified lookup
    ::func(b);     // (4), qualified lookup
    // func(f);    // ambiguous: (1) or (3), global and ns1
    func(b);       // (4), global and ns2
}
