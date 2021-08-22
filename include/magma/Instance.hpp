#pragma once

#include <magma/EngineInfo.hpp>
#include <magma/glfw/GlfwStack.hpp>
#include <magma/utils/Name.hpp>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

namespace magma {

/**
 * @brief Configuration for Vulkan's debug extensions and layers
 */
struct ContextDebugConfig {
    /**
     * @brief Enable the validation layer from Khronos
     */
    bool validationLayer = false;
    /**
     * @brief Enable the debug utils extension to get more information using the debug messenger
     */
    bool debugUtilsExtension = false;
    /**
     * @brief Enable verbose messages in the debug messenger
     */
    bool verbose = false;
};

/**
 * @brief Application level configuration of Magma engine
 */
struct ContextCreateInfo {
    /**
     * @brief List of layers to enable
     */
    std::vector<const char*> layers;
    /**
     * @brief List of extensions to enable
     */
    std::vector<const char*> extensions;
    /**
     * @brief Debug configuration to use
     */
    ContextDebugConfig debugConfig;
    /**
     * @brief Name of the application
     */
    const char* applicationName;
    /**
     * @brief Version of the application
     */
    uint32_t applicationVersion;
};

/**
 * @brief
 */
class Instance {
public:
    Instance(const ContextCreateInfo& createInfo);

    // TODO temporary, wrap vk::raii::SurfaceKHR into a RenderTarget class
    vk::raii::SurfaceKHR makeSurface(GLFWwindow* window);

    // TODO temporary, remove
    operator const vk::raii::Instance&() const noexcept { return instance_; }

private:
    vk::raii::Instance makeInstance() const;
    vk::raii::DebugUtilsMessengerEXT makeDebugMessenger() const;

    /**
     * @brief Wrap the ContextCreateInfo class to provide a constructor without changing its
     * aggregate trait
     */
    struct ContextCreateInfoWrapper final : ContextCreateInfo {
        ContextCreateInfoWrapper(const ContextCreateInfo& createInfo);
    };

    magma::GlfwStack glfw;
    ContextCreateInfoWrapper createInfo_;
    vk::raii::Context context_;
    vk::raii::Instance instance_;
    vk::raii::DebugUtilsMessengerEXT debugUtilsMessenger_;
};

} // namespace magma
