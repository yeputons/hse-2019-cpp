#include <type_traits>
#include <vector>

template<typename T> struct HasBeginImpl {
    struct yes { char a[1]; };  // На лекции размеры были наоборот, это неважно.
    struct no { char a[2]; };

    static no test(...) {
        return {};
    }
    template<typename U = T, std::void_t<decltype(std::declval<U>().begin())>* = nullptr>
    static yes test(std::nullptr_t) {
        return {};
    }
};

template<typename T>
constexpr bool HasBegin = sizeof(HasBeginImpl<T>::test(nullptr)) == sizeof(typename HasBeginImpl<T>::yes);

struct Foo { void begin() {} };

int main() {
    static_assert(!HasBegin<int>);
    static_assert(HasBegin<Foo>);
    static_assert(HasBegin<std::vector<int>>);
}
