#ifndef MEMORIZER_H
#define MEMORIZER_H

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace cls_30 {
template<typename R, typename ...Args>
struct CallLog {
    R result;
    std::tuple<Args...> arguments;
    CallLog(R result_, Args ...args) : result(std::move(result_)), arguments(std::move(args)...) {}
    CallLog(R result_, std::tuple<Args...> arguments_) : result(std::move(result_)), arguments(std::move(arguments_)) {}
};

template<typename ...Args>
struct CallLog<void, Args...> {
    std::tuple<Args...> arguments;
    CallLog(Args ...args) : arguments(std::move(args)...) {}
    CallLog(std::tuple<Args...> arguments_) : arguments(std::move(arguments_)) {}
};

namespace impl {
    template<typename Fn>
    struct function_traits {};
    template<typename R, typename ...Args>
    struct function_traits<R(*)(Args...)> {
        using call_result_type = CallLog<std::remove_reference_t<R>, std::remove_reference_t<Args>...>;
    };
}  // namespace impl

template<typename Fn, typename C = typename impl::function_traits<Fn>::call_result_type>
struct Memorizer {
    Fn fn;

    std::vector<C> calls;

    Memorizer(Fn fn_) : fn(std::move(fn_)) {}

    template<typename ...Args>
    decltype(auto) operator()(Args &&...args) {
        std::tuple<std::remove_reference_t<Args>...> arguments(args...);
        if constexpr (std::is_same_v<void, decltype(fn(args...))>) {
            fn(args...);
            calls.emplace_back(std::move(arguments));
        } else {
            decltype(auto) result = fn(args...);
            calls.emplace_back(result, std::move(arguments));
            return result;
        }
    }
};
}  // namespace cls_30

#endif  // MEMORIZER_H
