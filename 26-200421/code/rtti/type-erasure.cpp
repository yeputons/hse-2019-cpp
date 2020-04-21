#include <any>
#include <iostream>

using std::cout;

void print(const std::any &a) {
    if (auto *pInt = std::any_cast<int>(&a); pInt)
        cout << "int: " << *pInt << '\n';
}

int main() {
    print(10);     // int: 10
    print("foo");  //
}
