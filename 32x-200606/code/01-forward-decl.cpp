#include <iostream>

int dfs2(int v);

int dfs1(int v) {
    return dfs2(v - 1);
}

int dfs2(int v) {
    return dfs1(v - 1);
}

struct Bar;  // incomplete type
// 1
struct Foo {
    Bar *x;
    void foo();
};
// 2
struct Bar {
    Foo *x;
    void bar() {}
};
void Foo::foo() {
    Bar z;  // Bar should be complete type
    x->bar();  // Bar should be complete type
}

int main() {
    return 0;
}
