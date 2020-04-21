namespace ns {
    struct Foo {};
    struct Bar { Bar(Foo) {} };
    void foo() {
        Foo f;
        auto x = Bar(f);
    }
}
int main() {
    ns::Foo f;
    auto x = Bar(f);
}
