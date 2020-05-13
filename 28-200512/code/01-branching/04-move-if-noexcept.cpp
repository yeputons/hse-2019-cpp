#include <type_traits>

// Сейчас не хватает мощи, чтобы точно написать все нужные типы :(
// Будет на следующей лекции.
template<typename T>
??? move_if_noexcept(??? value) {
    if constexpr (std::is_noexcept_move_assignable_v<T>) {
        return std::move(value);
    } else {
        return value;
    }
}
