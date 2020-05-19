#ifndef OPTIONAL_H_
#define OPTIONAL_H_

#include <memory>
#include <type_traits>
#include <utility>

namespace cls_29 {

template<typename T>
class optional {
    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    bool created = false;

public:
    optional() {}

    optional(const T &v) : created(true) {
        new (&storage) T(v);
    }

    optional(T &&v) : created(true) {
        new (&storage) T(std::move(v));
    }

    T& value() & noexcept { return reinterpret_cast<T&>(storage); }
    T&& value() && noexcept { return reinterpret_cast<T&&>(storage); }
    const T& value() const& noexcept { return reinterpret_cast<const T&>(storage); }

    bool has_value() const noexcept { return created; }

    optional(const optional &other) : created(other.created) {
        if (created) {
            new (&storage) T(other.value());
        }
    }

    optional(optional &&other) : created(other.created) {
        if (created) {
            new (&storage) T(std::move(other).value());
        }
    }

    optional& operator=(optional other) {
        if (!other.created) {
            reset();
        } else {
            if (created) {
                value() = std::move(other).value();
            } else {
                new (&storage) T(std::move(other).value());
                created = true;  // Basic exception safety: flag is set after construction.
            }
        }
        return *this;
    }

    ~optional() {
        reset();
    }

    void reset() noexcept {
        if (created) {
            value().~T();
            created = false;
        }
    }

    template<typename Arg0>
    void emplace1ByValue(Arg0 arg0) {
        reset();  // No strong exception safety :(
        new (&storage) T(std::move(arg0));
        created = true;  // Basic exception safety: flag is set after construction.
    }

    template<typename ...Args>
    void emplace(Args &&...args) {
        reset();
        new (&storage) T(std::forward<Args>(args)...);
        created = true;
    }
};

}  // namespace cls_29

#endif  // OPTIONAL_H_
