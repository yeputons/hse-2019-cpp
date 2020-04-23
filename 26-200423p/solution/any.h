#ifndef ANY_H_
#define ANY_H_

#include <stdexcept>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace cls_26 {

struct bad_any_cast : std::logic_error {
    bad_any_cast() : std::logic_error("Bad any_cast") {}
};

struct any {
    any() {}

    any(any &&other) = default;

    any(const any &other) : data(other.data ? other.data->clone() : nullptr) {}

    template<typename T>
    any(const T &object) : data(std::make_unique<any_data<T>>(object)) {}

    any& operator=(any other) {
        data.swap(other.data);
        return *this;
    }

    const std::type_info& type() const {
        return data ? data->type() : typeid(void);
    }

    template<typename T>
    friend T* any_cast(any *a) {
        return a && a->data && a->data->type() == typeid(T) ? static_cast<T*>(a->data->data()) : nullptr;
    }

    template<typename T>
    friend const T* any_cast(const any *a) {
        return a && a->data && a->data->type() == typeid(T) ? static_cast<T*>(a->data->data()) : nullptr;
    }

private:
    struct any_data_base {
        virtual ~any_data_base() {}
        virtual const std::type_info& type() const noexcept = 0;
        virtual std::unique_ptr<any_data_base> clone() const = 0;
        virtual void* data() noexcept = 0;
        virtual const void* data() const noexcept = 0;
    };

    std::unique_ptr<any_data_base> data;

    template<typename T>
    struct any_data : any_data_base {
        any_data(const T &object_) : object(object_) {}

        const std::type_info& type() const noexcept override {
            return typeid(T);
        }

        std::unique_ptr<any_data_base> clone() const override {
            return std::make_unique<any_data<T>>(object);
        }

        void* data() noexcept override {
            return &object;
        }

        const void* data() const noexcept override {
            return &object;
        }

    private:
        T object;
    };
};

template<typename T> T* any_cast(any*);
template<typename T> const T* any_cast(const any*);

template<typename T>
T any_cast(any &a) {
    auto ptr = any_cast<std::remove_reference_t<T>>(&a);
    if (!ptr) throw bad_any_cast();
    return *ptr;
}

template<typename T>
T any_cast(const any &a) {
    auto ptr = any_cast<T>(&a);
    if (!ptr) throw bad_any_cast();
    return *ptr;
}

template<typename T>
T any_cast(any &&a) {
    auto ptr = any_cast<T>(&a);
    if (!ptr) throw bad_any_cast();
    return std::move(*ptr);
}

}  // namespace cls_26

#endif  // ANY_H_
