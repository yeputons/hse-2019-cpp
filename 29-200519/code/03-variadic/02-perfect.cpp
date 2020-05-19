#include <iostream>
#include <tuple>
#include <utility>

template<typename Fn, typename ...Args>
decltype(auto) wrapper(Fn fn, Args &&...args) {
    struct Wrapper {
        Wrapper() {
            std::cout << "before\n";
        }

        ~Wrapper() {
            std::cout << "after\n";
        }
    } w;
    return std::forward<Fn>(fn)(std::forward<Args>(args)...);
}

int main() {
    struct {
        int field;

        int &operator()(int x, std::string &s) && {
            std::cout << "working\n";
            field = x + 2;
            s += "y";
            return field;
        }
    } fn;

    std::string s("x");
    int &f = wrapper(fn, 10, s);
    std::cout << s << "\n";
    std::cout << f << "\n";
}
