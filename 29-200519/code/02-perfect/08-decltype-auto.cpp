decltype(auto) foo() {
    int x = 10;
    return x;
}

decltype(auto) bar() {
    int x = 10;
    return (x);
}

decltype(auto) baz() {

}

int main() {
    {
        int x = foo();
        int &y = bar();
        baz();
    }

    {
        auto wrapperFoo = []() -> decltype(auto) { return foo(); };
        auto wrapperBar = []() -> decltype(auto) { return bar(); };
        auto wrapperBaz = []() -> decltype(auto) { return baz(); };  // `return void` is ok
        int x = wrapperFoo();
        int &y = wrapperBar();
        wrapperBaz();
        // auto z = wrapperBaz();  // `auto=void` is not ok; use `if constexpr`.
    }
}