#pragma once

#include <magma/utils/Algorithm.hpp>

#include <vector>

namespace utils::name {

/**
 * @brief Appends each element of names into list if not already present
 */
void appendIfNotPresent(std::vector<const char*>& list, const std::vector<const char*>& names);

} // namespace utils::name
