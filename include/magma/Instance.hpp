#pragma once

#include <magma/EngineInfo.hpp>
#include <magma/glfw/GlfwStack.hpp>
#include <magma/stdx/Algorithm.hpp>
#include <magma/stdx/Name.hpp>

#include <magma/Vulkan.hpp>

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
     * @brief Enable the debug utils extension to get more information for debug purposes
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
    operator const vk::raii::Instance&() const noexcept
    {
        return instance_;
    }

    vk::raii::PhysicalDevice pickPhysicalDevice(const vk::raii::SurfaceKHR& surface) const;

    template<typename TPhysicalDevicePicker>
    vk::raii::PhysicalDevice pickPhysicalDevice(
        const vk::raii::SurfaceKHR& surface,
        TPhysicalDevicePicker&& pick) const;

private:
    static bool isDeviceCompatible(
        const vk::raii::PhysicalDevice& device,
        const vk::raii::SurfaceKHR& surface);

    void removeIncompatiblePhysicalDevices(
        vk::raii::PhysicalDevices& devices,
        const vk::raii::SurfaceKHR& surface) const;

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

#include <magma/Instance.inl>
