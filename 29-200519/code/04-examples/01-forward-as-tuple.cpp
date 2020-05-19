#include <functional>
#include <string>
#include <tuple>
#include <utility>

template<typename> struct TD;

struct Foo {};

template<typename ...Args>
std::tuple<Args...> my_make_tuple(Args ...args) {
    return std::tuple<Args...>(std::move(args)...);
}

template<typename ...Args>
std::tuple<Args&&...> my_forward_as_tuple(Args &&...args) {
    return std::tuple<Args&&...>(std::forward<Args>(args)...);
}

void foo(int x, Foo &&f) {
}

int main() {
    int x = 10;
    auto t1 = my_make_tuple(x, Foo());
    auto t2 = my_forward_as_tuple(x, Foo());
    std::apply(foo, t1);
    std::apply(foo, t2);
//    TD<decltype(t1)>{};
//    TD<decltype(t2)>{};
}