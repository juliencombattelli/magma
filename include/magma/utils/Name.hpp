#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

namespace utils::name {

/**
 * @brief Utility functions to help working with list of extension and layer names
 */

bool contains(const std::vector<const char*>& names, const char* name);

void appendIfNotPresent(std::vector<const char*>& list, const std::vector<const char*>& names);

} // namespace utils::name
