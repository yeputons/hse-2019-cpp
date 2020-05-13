#include <type_traits>
#include <vector>

// Не было на лекции!
template<typename T> struct HasBeginImpl {
    struct yes { char a[1]; };
    struct no { char a[2]; };

    static no test(...) {
        return {};
    }

    template<typename H, H value> struct Helper { typedef void type; };

    template<typename U = T, typename Helper<typename U::const_iterator (U::*)() const noexcept, &U::begin>::type* = nullptr>
    static yes test(std::nullptr_t) {
        return {};
    }
};

template<typename T>
constexpr bool HasBegin = sizeof(HasBeginImpl<T>::test(nullptr)) == sizeof(typename HasBeginImpl<T>::yes);

struct Foo { void begin() {} };

int main() {
    static_assert(!HasBegin<int>);
    static_assert(!HasBegin<Foo>);  // Изменилось: возвращает теперь не void.
    static_assert(HasBegin<std::vector<int>>);
}
