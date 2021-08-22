#pragma once

#include <vulkan/vulkan_core.h>

namespace magma {

struct EngineInfo {
    static constexpr inline const char* name = "Magma";
    // TODO generate from CMake
    static constexpr inline uint32_t version = VK_MAKE_API_VERSION(0, 1, 0, 0);
};

} // namespace magma
