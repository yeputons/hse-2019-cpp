#include <cassert>
#include <functional>
#include <iostream>
#include <typeinfo>

using std::cout;

struct Base { virtual ~Base() {} int x; };
struct Derived : Base { int y; };

int main() {
    {
        Derived d;
        Base *b = &d;
        const std::type_info &info_b = typeid(*b);  // Всегда(!) по const&
        assert(info_b == typeid(d));
        assert(info_b == typeid(Derived));
        assert(info_b != typeid(Derived*));
        cout << info_b.name() << '\n';
    }
    {
        std::function<int(int)> f;  // Какого размера f?
        int data[100] = {};
        f = [](int a) { return a + 2; };      // Лямбда размера 0.
        cout << f(10) << '\n';  // 12
        f = [data](int) { return data[0]; };  // Лямбда размера 400.
        cout << f(10) << '\n';  // 50
    }
}
