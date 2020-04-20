#include "foo.h"
#include <iostream>

const int Foo::N;            // Можно инициализировать ровно в одном
constexpr char Foo::Name[];  // из двух мест: объявление или определение

void foo() {
    std::cout << Foo::N << '\n';
    std::cout << Foo::Name[0] <<'\n';
    std::cout << &Foo::N << '\n';
    std::cout << &Foo::Name[0] << '\n';
}
