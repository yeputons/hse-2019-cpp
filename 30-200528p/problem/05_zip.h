#ifndef ZIP_H
#define ZIP_H

#include <cstddef>
#include <tuple>
#include <utility>
#include <type_traits>

namespace cls_30 {
template<typename ...Ts, typename ...Us, std::enable_if_t<sizeof...(Ts) == sizeof...(Us)>* = nullptr>
auto zip(const std::tuple<Ts...> &a, const std::tuple<Us...> &b) {
    static_assert(sizeof...(Ts) == sizeof...(Us));
    return [&]<std::size_t ...Indices>(std::index_sequence<Indices...>) {
        return std::tuple(
            std::pair(std::get<Indices>(a), std::get<Indices>(b))...
        );
    }(std::make_index_sequence<sizeof...(Ts)>());
}
}  // namespace cls_30

#endif  // ZIP_H
