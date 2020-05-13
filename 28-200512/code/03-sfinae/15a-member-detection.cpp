#include <type_traits>
#include <vector>

template<typename T> struct HasBeginImpl {
    static constexpr bool test(...) {
        return false;
    }
    template<typename U = T, std::void_t<decltype(std::declval<U>().begin())>* = nullptr>
    static constexpr bool test(std::nullptr_t) {
        return true;
    }
};

template<typename T>
constexpr bool HasBegin = HasBeginImpl<T>::test(nullptr);

struct Foo { void begin() {} };

int main() {
    static_assert(!HasBegin<int>);
    static_assert(HasBegin<Foo>);
    static_assert(HasBegin<std::vector<int>>);
}
