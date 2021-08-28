#pragma once

#include <algorithm>

namespace utils {

template<typename TContainer, typename TValue, typename TProjector = std::identity>
bool contains(const TContainer& container, const TValue& value, TProjector projector = {}) {
    return std::ranges::find(container, value, projector) != container.end();
}

} // namespace utils
