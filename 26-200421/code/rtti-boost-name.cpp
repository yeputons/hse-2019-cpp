#include <boost/core/demangle.hpp>
#include <typeinfo>
#include <iostream>

template<class T> struct X
{
};

int main()
{
    char const * name = typeid(X<int>).name();

    std::cout << name << std::endl;  // prints 1XIiE
    std::cout << boost::core::demangle( name ) << std::endl;  // prints X<int>
}
