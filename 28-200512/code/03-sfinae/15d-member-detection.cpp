#include <type_traits>
#include <vector>

// Не было на лекции!

template<typename T>
constexpr bool HasBegin = requires (const T &value) { value.begin(); };

struct Foo { void begin() {} };

int main() {
    static_assert(!HasBegin<int>);
    static_assert(!HasBegin<Foo>);  // Изменилось: возвращает теперь не void.
    static_assert(HasBegin<std::vector<int>>);
}
