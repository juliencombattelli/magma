#pragma once

#include <magma/Version.hpp>

#include <vulkan/vulkan_core.h>

namespace magma {

struct EngineInfo {
    static constexpr inline const char* name = "Magma";
    static constexpr inline uint32_t version
        = VK_MAKE_API_VERSION(0, Version::major, Version::minor, Version::patch);
};

} // namespace magma
