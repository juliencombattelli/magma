#pragma once

#include <magma/stdx/Algorithm.hpp>

#include <vector>

namespace magma::stdx {

/**
 * @brief Appends each element of names into list if not already present
 */
void appendIfNotPresent(std::vector<const char*>& list, const std::vector<const char*>& names);

} // namespace magma::stdx
