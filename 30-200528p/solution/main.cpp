#include <cassert>
#include <functional>
#include <iostream>
#include <utility>
#include <type_traits>
#include <string_view>
#include <sstream>

struct unique_ptr_new_t {};
const inline unique_ptr_new_t unique_ptr_new;

template<typename T>
class unique_ptr {
    T *data = nullptr;
public:
    unique_ptr() {}
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr(unique_ptr &&other) : data(other.data) {
        other.data = nullptr;
    }
    unique_ptr& operator=(unique_ptr other) {
        std::swap(data, other.data);
        return *this;
    }
    ~unique_ptr() {
        delete data;
    }

    T& operator*() { return *data; }
    const T& operator*() const { return *data; }

    // START SOLUTIOn
    template<typename ...Args>
    unique_ptr(unique_ptr_new_t, Args &&...args)
        : data(new T(std::forward<Args>(args)...))
    {}
    // END SOLUTION
};

template<typename ...Args>
std::string concat(const Args &...args) {
    // START SOLUTION
    std::stringstream s;
    (s << ... << args);
    return s.str();
    // END SOLUTION
}

void concat_separated_impl(bool, std::stringstream &, std::string_view) {
}

template<typename Arg, typename ...Args>
void concat_separated_impl(bool first, std::stringstream &s, std::string_view sep, const Arg &arg, const Args &...args) {
    if (!first) s << sep;
    s << arg;
    concat_separated_impl(/*first=*/false, s, sep, args...);
}

template<typename ...Args>
std::string concat_separated(std::string_view sep, const Args &...args) {
    std::stringstream s;
    concat_separated_impl(/*first=*/true, s, sep, args...);
    return s.str();
}

template<typename ...Args>
std::string concat_separated(std::string_view sep, const std::tuple<Args...> &t) {
    return std::apply(
        static_cast<std::string(*)(std::string_view, const Args&...)>(concat_separated),
        std::tuple_cat(std::tuple(sep), t)
    );
}

template<typename ...Args1, typename ...Args2>
auto zip(const std::tuple<Args1...> &t1, const std::tuple<Args2...> &t2) {
    static_assert(sizeof...(Args1) == sizeof...(Args2));
    return [&]<std::size_t ...Indices>(std::index_sequence<Indices...>){
        return std::tuple(std::pair(std::get<Indices>(t1), std::get<Indices>(t2))...);
    }(std::make_index_sequence<sizeof...(Args1)>());
}

template<typename T> decltype(auto) apply_vector_helper(T v) { return v; }

template<typename R, typename ...Args, typename T>
decltype(auto) apply_vector(R(*fn)(Args...), const std::vector<T> &vec) {
    assert(vec.size() == sizeof...(Args));
    // GCC bug: https://stackoverflow.com/questions/36852367/are-there-sequence-points-in-braced-initializer-lists-when-they-apply-to-constru#comment63834950_38172664
//    int i = 0;
//    return std::apply(fn, std::tuple{apply_vector_helper<Args>(vec[i++])...});
    return [&]<std::size_t ...Indices>(std::index_sequence<Indices...>) {
        return std::invoke(fn, vec[Indices]...);
    }(std::make_index_sequence<sizeof...(Args)>());
}

template<typename T, typename U, typename ...Args>
decltype(auto) get_arg(U &&v, Args &&...args) {
    if constexpr (std::is_same_v<T&&, U&&>) {
        return v;
    } else {
        return get_arg<T>(std::forward<Args>(args)...);
    }
}

template<typename R, typename ...EArgs, typename ...RArgs>
R reorder_args(R(*fn)(EArgs...), RArgs &&...args) {
    return fn(get_arg<EArgs>(std::forward<RArgs>(args)...)...);
}

void foo(int a, char b, std::string c, int &d) {
    std::cout << "a=" << a << "; ";
    std::cout << "b=" << b << "; ";
    std::cout << "c=" << c << "; ";
    std::cout << "d=" << d << "\n";
}

void bar(int a, int b, int c, int d) {
    std::cout << "a=" << a << "; ";
    std::cout << "b=" << b << "; ";
    std::cout << "c=" << c << "; ";
    std::cout << "d=" << d << "\n";
}

template<typename...> struct TD;

template<typename Fn, typename R, typename ...Args>
struct Mock {
    Fn fn;
    std::vector<std::tuple<R, std::remove_reference_t<Args>...>> calls;

    Mock(Fn fn_) : fn(fn_) {}
    R operator()(Args ...args) {
        calls.emplace_back(fn(args...), args...);
        return std::get<0>(calls.back());
    }
};
template<typename R, typename ...Args> Mock(R(*)(Args...)) -> Mock<R(*)(Args...), R, Args...>;

int my_func(int &a, int b, int arr[]) {
    return a * 10 + b;
}

int main() {
    unique_ptr<std::string> xxx(unique_ptr_new, 10, 'x');
    std::cout << *xxx << "\n";

    std::cout << "|" << concat("hello", 10, "world") << "|\n";
    std::cout << "|" << concat_separated(", ", "hello", 10, "world") << "|\n";
    std::cout << "|" << concat_separated(", ", std::tuple("hello", 10, "world")) << "|\n";
    apply_vector(bar, std::vector{10, 20, 30, 40});

    int z = 40;
    reorder_args(foo, std::string("hello"), 33, 'x', z);
    reorder_args(foo, z, std::string("hello"), 33, 'x');

    assert(zip(std::tuple(10, std::string("x")), std::tuple(std::string("y"), 20)) ==
        std::tuple(
            std::pair(10, std::string("y")),
            std::pair(std::string("x"), 20)
        )
    );

    {
        Mock m(my_func);
        int a = 10;
        int arr[10];
        std::cout << m(a, 2, arr) << "\n";
        a++;
        std::cout << m(a, 3, arr) << "\n";
        a++;
        for (const auto &x : m.calls) {
            std::cout << "(" << std::get<1>(x) << ", " << std::get<2>(x) << ") = " << std::get<0>(x) << "\n";
        }
    }
}

/*
Ещё идеи:

* Теперь можно сделать объект `log`, который имеет `operator()` и логирует все вызовы и аргументы
  (если они форматируемые), при этом делает perfect forward и сам следит за вложенностью отступов.
* Распад аргументов (надо при сохранении в поля?)
  * `std::array` (не decay’ится)
  * `decay_t` вместо `remove_cvref`
*/
