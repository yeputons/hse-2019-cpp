#include <iostream>
#include <memory>
#include <string>

template<typename T>
struct MySmartPointer {
    std::unique_ptr<T> x = std::make_unique<T>();

    T& operator*() { return *x; }
    const T& operator*() const { return *x; }

    // Asymmetry :(
//    T* operator->() { return x.get(); }
//    const T* operator->() const { return x.get(); }

    // Better
//    std::unique_ptr<T>& operator->() { return x; }
//    const std::unique_ptr<T>& operator->() const { return x; }

    // Probably even better.
    auto& operator->() { return x; }
    /*const*/ auto& operator->() const { return x; }
};

int main() {
    MySmartPointer<std::string> ptr;
    *ptr = "hello";
    std::cout << "|" << *ptr << "|\n";
    std::cout << ptr->length() << "\n";
}
