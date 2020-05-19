#include <iostream>
#include <tuple>
#include <utility>

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

    int calls = 0;
#if 0
    auto wrapper = [&calls]<typename Fn, typename ...Args>(Fn &&fn, Args &&...args) -> decltype(auto) {
            calls++;
            return std::forward<Fn>(fn)(std::forward<Args>(args)...);
    };
#else
    auto wrapper = [&calls](auto &&fn, auto &&...args) -> decltype(auto) {
        calls++;
        // static_cast is enough; std::forward clarifies intention.
        return std::forward<decltype(fn)>(fn)(std::forward<decltype(args)>(args)...);
    };
#endif

    std::string s("x");
    int &f = wrapper(std::move(fn), 10, s);
    std::cout << s << "\n";
    std::cout << f << "\n";
    std::cout << "calls=" << calls << "\n";
}
