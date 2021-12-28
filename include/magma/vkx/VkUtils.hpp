#pragma once

#include <magma/stdx/TypeTraits.hpp>

namespace magma::vkx {

template<typename TFlag, typename... TFlags>
auto setFlags(TFlag firstFlag, TFlags... otherFlags)
{
    static_assert(sizeof...(TFlags) != 0, "At least two arguments must be provided");
    static_assert(stdx::areSame_v<TFlag, TFlags...>, "The flags provided must have the same type");
    return firstFlag | (otherFlags | ...);
}

} // namespace magma::vkx
