#pragma once

#include <magma/EngineInfo.hpp>
#include <magma/glfw/GlfwStack.hpp>
#include <magma/utils/Algorithm.hpp>
#include <magma/utils/Name.hpp>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <iostream> // TODO remove

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

    vk::raii::PhysicalDevice pickPhysicalDevice() const {
        auto defaultPicker
            = [](const vk::raii::PhysicalDevices& devices) -> const vk::raii::PhysicalDevice& {
            return devices.front();
        };
        return pickPhysicalDevice(defaultPicker);
    }

    template<typename TPhysicalDevicePicker>
    vk::raii::PhysicalDevice pickPhysicalDevice(TPhysicalDevicePicker&& pick) const {
        vk::raii::PhysicalDevices devices(instance_);
        removeIncompatiblePhysicalDevices(devices);
        const auto& pickedDevice = pick(devices);
        // A move is not possible as it would fallback to a copy since pickedDevice is const. And
        // pick function has to return a const PhysicalDevice&, otherwise implementation would be
        // unecessary complicated for the user:
        //   - returning by value would force the user to handle the move inside pick
        //   - returning a non-const& would require passing to pick the vector by value (expansive),
        //     or by non-const& (the vector should stay read only)
        // Thus we create a new raii object from the vkPhysicalDevice C handler.
        return vk::raii::PhysicalDevice(instance_, *pickedDevice);
    }

private:
    static bool isDeviceCompatible(const vk::raii::PhysicalDevice& device) {
        static const std::vector<std::string_view> requiredExtensions
            = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        const auto availableExtensions = device.enumerateDeviceExtensionProperties();
        for (const auto extension : requiredExtensions) {
            if (!utils::contains(
                    availableExtensions, extension, &vk::ExtensionProperties::extensionName)) {
                std::cerr << extension << " not present\n";
                std::cerr << device.getProperties().deviceName << " is not compatible\n";
                return false;
            }
        }
        return true;
    }
    void removeIncompatiblePhysicalDevices(vk::raii::PhysicalDevices& devices) const {
        auto incompatibleCount
            = std::erase_if(devices, [](auto&& device) { return !isDeviceCompatible(device); });
        std::cout << "Removed " << incompatibleCount << " devices\n";
    }

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
