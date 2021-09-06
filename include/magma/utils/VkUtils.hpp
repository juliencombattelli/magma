#pragma once

#include <magma/utils/TypeTraits.hpp>

namespace magma::vkutils {

template<typename TFlag, typename... TFlags>
auto setFlags(TFlag firstFlag, TFlags... otherFlags)
{
    static_assert(sizeof...(TFlags) != 0, "At least two arguments must be provided");
    static_assert(areSame_v<TFlag, TFlags...>, "The flags provided must have the same type");
    return firstFlag | (otherFlags | ...);
}

} // namespace magma::vkutils
