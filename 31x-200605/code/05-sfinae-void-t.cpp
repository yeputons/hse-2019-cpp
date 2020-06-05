#include <iostream>
#include <memory>
#include <type_traits>

template<typename T, typename = void>
struct MyFormatHelper {
    void func1() {}
};

template<typename T>
struct MyFormatHelper<T, decltype(static_cast<void>(std::declval<T>().foo()))> {
    void func2() {}
};

struct Foo {
    void foo() {}
};

int main() {
    MyFormatHelper<int> f1;  // 1
    f1.func1();
    MyFormatHelper<Foo> f2;  // 2
    f2.func2();
    return 0;
}
