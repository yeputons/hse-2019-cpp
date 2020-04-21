#include <algorithm>
#include <tuple>
namespace root {
    namespace ns1 { struct Base1 {}; void func1(...) {} }
    namespace ns2 { struct Base2 {}; void func2(...) {} }
    namespace ns3 {
        void func3(...) {}
        struct Container : ns1::Base1 {
            static void func4(...) {}
            struct Derived : ns2::Base2 {};
        };
    }
    void func5(...) {}
    using Derived = ns3::Container::Derived;
}
int main() {
    void (*data)(std::tuple<root::Derived*>) = nullptr;
    // sort(data);  // std::sort found, compilation error
    // func1(data);  // compilation error
    func2(data);  // ok
    func3(data);  // ok
    // func4(data);  // compilation error
    // func5(data); // compilation error
}
