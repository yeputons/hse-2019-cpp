#include <iostream>
#include <vector>
#include <type_traits>

using namespace std;

struct Foo {

};

Foo pushed;

void push1(Foo arg) {
    pushed = std::move(arg);
}

void demo1() {
    Foo f;
    push1(f);      // 1. copy to 'arg'; 2. move 'arg' to 'pushed'; 3. destroy 'arg'
    push1(Foo());  // 1. move to 'arg'; 2. move 'arg' to 'pushed'; 3. destroy 'arg'
}

// More effective; akin to with perfect forwarding
void push2(const Foo &arg) {
    pushed = arg;
}

void push2(Foo &&arg) {
    pushed = std::move(arg);
}

void demo2() {
    Foo f;
    push2(f);      // 1. Reference is passed to 'arg'. 2. Copy 'f' to 'pushed'.
    push2(Foo());  // 1. Reference is passed to 'arg'. 2. Move 'Foo()' to 'pushed'. | after push(): 3. destroy 'Foo()'
}

int main() {
    return 0;
}
