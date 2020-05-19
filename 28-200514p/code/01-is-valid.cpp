#include <boost/hana.hpp>
#include <cassert>
#include <iostream>

void foo(int) {}
void foo(char) {}

template<typename> constexpr bool true_v = true;
//template<typename T> bool is_same_v<int, T> = is_same<int, T>::value;

template<typename Fn>
struct Foo {
//    T* t;
//    Foo() {}

//    template<typename U, typename = std::void_t<decltype(std::declval<Fn>()( std::declval<U>() ))>>
//    template<typename U, std::void_t<decltype(std::declval<Fn>()( std::declval<U>() ))>* = nullptr>
/*    template<typename U>
    std::enable_if_t<
       true_v<decltype(std::declval<Fn>()( std::declval<U>() ))> &&
       true_v<decltype(std::declval<Fn>()( std::declval<U>() ))>
       ,
       bool
    > operator()(U) {
        return true;
    }*/

    template<typename U>
    decltype(
       std::declval<Fn>()( std::declval<U>() ),
       //2 * 2 == 5,
       std::enable_if_t<2 * 2 == 4, int>{},
       /*true*/ /*std::declval<bool>()*/
       bool{}
    ) operator()(U) {
        return true;
    }

//    template<typename U>
    bool operator()(...) {
        return false;
    }
};

template<typename Fn>
auto is_valid(Fn) {
    return Foo<Fn>{};
}

int main() {
    auto f = is_valid(   [](auto x) -> decltype(foo(x)) {}   );
    assert(f(10));
    assert(!f("xoo"));

    // auto g = is_valid(  [](){}  );
    // assert(g());

/*    std::cout << boost::hana::is_valid(
        [](auto x) -> decltype(foo(x)) {}
    )("xoo") << "\n";*/
}
