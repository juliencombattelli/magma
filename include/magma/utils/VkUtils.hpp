#pragma once

namespace magma::vkutils {

template<typename TFlag, typename... TFlags>
// TODO add require clause for all same types and TFlags not empty
auto setFlags(TFlag firstFlag, TFlags... otherFlags)
{

    return firstFlag | (otherFlags | ...);
}

} // namespace magma::vkutils
