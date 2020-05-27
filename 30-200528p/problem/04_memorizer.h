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
    // TODO: CallLog(result, arguments....) consturctor
};

namespace impl {
    template<typename Fn>
    struct function_traits {};
    // TODO: specialize for function pointers
}  // namespace impl

template<typename Fn, typename C>
struct Memorizer {
    Fn fn;

    std::vector<C> calls;

    Memorizer(Fn fn_) : fn(std::move(fn_)) {}

    // TODO: overload operator() for arbitrary arguments
};
}  // namespace cls_30

#endif  // MEMORIZER_H
