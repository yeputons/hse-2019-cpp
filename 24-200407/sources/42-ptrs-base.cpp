#include <memory>

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::static_pointer_cast;
using std::unique_ptr;

struct Base {
    ~Base() {}  // Обязательно, потому что собираемся хранить указатель на Base => наверняка можем удалить с любым статическим типом.
};

struct Derived : Base {
};

int main() {
    {
        auto der = make_unique<Derived>();
        unique_ptr<Base> base = std::move(der);
        unique_ptr<Derived> der2(static_cast<Derived*>(base.release()));
    }
    {
        auto der = make_shared<Derived>();
        shared_ptr<Base> base = der;
        shared_ptr<Derived> der2 = static_pointer_cast<Derived>(base);
    }
    return 0;
}
