namespace foo {
    namespace impl {
        struct Foo { int x; };
        int func(const Foo &f) { return f.x; }
        int foo(const Foo &f) { return f.x; }
    }
    using impl::Foo;
}
namespace bar::impl {
    struct Bar{ int x; };
    int func(const Bar &f) { return f.x; }
}

int main() {
    foo::Foo f;
    bar::impl::Bar b;
    func(f);  // ok
    func(b);  // ok
    // foo::impl::foo(f);  // Qualified lookup, no ADL, ok.
    // foo::foo(f);  // Qualified lookup, no ADL, compilation error.
    // foo(f);  // compilation error: namespace foo
}
