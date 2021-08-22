#include <magma/utils/Name.hpp>

namespace utils::name {

bool contains(const std::vector<const char*>& names, const char* name) {
    return std::find_if(names.begin(), names.end(), [&](const char* element) {
        return std::strcmp(element, name) == 0;
    }) != names.end();
}

void appendIfNotPresent(std::vector<const char*>& list, const std::vector<const char*>& names) {
    for (const char* name : names) {
        if (!contains(list, name)) {
            list.push_back(name);
        }
    }
}

} // namespace utils::name
