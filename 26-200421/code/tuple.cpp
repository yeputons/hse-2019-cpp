#include <cassert>
#include <functional>
#include <tuple>
#include <string>
#include <vector>

using std::string;
using std::vector;

void foo(int a, string b) {
    assert(a == 10);
    assert(b == "hello");
}

std::tuple<int, string> bar() {
    return {30, "baz"};
}

int main() {
    {
        std::tuple<int, vector<int>, string> t(10, vector<int>(2), "foo");
        auto t2 = std::make_tuple(10, vector<int>(2), "foo");  // t == t2
        assert(t == t2);
        int a = std::get<0>(t);  // Должна быть константа времени компиляции, циклов нет.
        string c = std::get<2>(t);
    }

    {
        std::pair<int, string> p(10, "foo");
        std::tuple<int, string> t = p;  // Неявная конверсия pair --> tuple.
        auto tt = std::tuple_cat(t, t);
        // Можно явно узнать тип элемента.
        std::tuple_element_t<0, decltype(tt)> x = std::get<0>(tt);
        static_assert(std::tuple_size_v<decltype(tt)> == 4);
    }

    {
        auto t = std::make_tuple(10, "hello");
        std::apply(foo, t);
    }

    {
        int a = 10; string b = "foo";
        std::tuple<int&, string&> t(a, b);
        t = std::make_tuple(20, "bar");  // a == 20, b == "bar"
        assert(a == 20);
        assert(b == "bar");

        std::tie(a, b) = bar();  // a == 30, b == "baz"
        assert(a == 30);
        assert(b == "baz");
    }
}
