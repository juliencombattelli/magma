#include <magma/utils/Name.hpp>

namespace utils::name {

void appendIfNotPresent(std::vector<const char*>& list, const std::vector<const char*>& names)
{
    for (const char* name : names) {
        if (!contains(list, name)) {
            list.push_back(name);
        }
    }
}

} // namespace utils::name
