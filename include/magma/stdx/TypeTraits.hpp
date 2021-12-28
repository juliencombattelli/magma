#pragma once

#include <type_traits>

namespace magma::stdx {

/**
 * @brief Type trait checking if all given types are identical
 */
template<typename TFirst, typename... T>
struct areSame {
    static constexpr bool value = std::conjunction_v<std::is_same<TFirst, T>...>;
};
template<typename... T>
inline constexpr bool areSame_v = areSame<T...>::value;

} // namespace magma::stdx
