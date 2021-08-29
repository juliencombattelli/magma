#include <magma/Instance.hpp>

#include <algorithm>
#include <iostream>
#include <ranges>
#include <span>

#ifndef VK_LAY_KHRONOS_VALIDATION_LAYER_NAME
#define VK_LAY_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

namespace magma {

// TODO consider policy class for missing extensions handlers
static void logNotSupportedExtensions(const std::vector<const char*>& extensions) {
    const auto supportedExtensions = vk::enumerateInstanceExtensionProperties();
    for (const auto& requestedExtension : extensions) {
        if (std::ranges::find_if(
                supportedExtensions,
                [&](const char* element) { return strcmp(element, requestedExtension) == 0; },
                &vk::ExtensionProperties::extensionName)
            == std::end(supportedExtensions)) {
            std::cerr << "Extension " << requestedExtension << " not supported\n";
        }
    }
}
static void logNotSupportedLayers(const std::vector<const char*>& layers) {
    const auto supportedLayers = vk::enumerateInstanceLayerProperties();
    for (const auto& requestedLayer : layers) {
        if (std::ranges::find_if(
                supportedLayers,
                [&](const char* element) { return strcmp(element, requestedLayer) == 0; },
                &vk::LayerProperties::layerName)
            == std::end(supportedLayers)) {
            std::cerr << "Layer " << requestedLayer << " not supported\n";
        }
    }
}

Instance::Instance(const ContextCreateInfo& createInfo)
    : createInfo_(createInfo)
    , instance_(makeInstance())
    , debugUtilsMessenger_(makeDebugMessenger()) {}

vk::raii::SurfaceKHR Instance::makeSurface(GLFWwindow* window) {
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(*instance_, window, nullptr, &surface);
    return vk::raii::SurfaceKHR(instance_, surface);
}

vk::raii::Instance Instance::makeInstance() const {
    vk::ApplicationInfo appInfo{
        .pApplicationName = createInfo_.applicationName,
        .applicationVersion = createInfo_.applicationVersion,
        .pEngineName = magma::EngineInfo::name,
        .engineVersion = magma::EngineInfo::version,
        .apiVersion = VK_API_VERSION_1_2,
    };
    vk::InstanceCreateInfo instanceCreateInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(createInfo_.layers.size()),
        .ppEnabledLayerNames = createInfo_.layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(createInfo_.extensions.size()),
        .ppEnabledExtensionNames = createInfo_.extensions.data(),
    };
    logNotSupportedExtensions(createInfo_.extensions);
    logNotSupportedLayers(createInfo_.layers);
    return vk::raii::Instance(context_, instanceCreateInfo);
}

vk::raii::DebugUtilsMessengerEXT Instance::makeDebugMessenger() const {
    if (createInfo_.debugConfig.debugUtilsExtension) {
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo{
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = debugCallback,
        };
        if (createInfo_.debugConfig.verbose) {
            debugUtilsMessengerInfo.messageSeverity
                |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
        }
        return vk::raii::DebugUtilsMessengerEXT(instance_, debugUtilsMessengerInfo);
    }
    return vk::raii::DebugUtilsMessengerEXT(instance_, VK_NULL_HANDLE);
}

Instance::ContextCreateInfoWrapper::ContextCreateInfoWrapper(const ContextCreateInfo& createInfo)
    : ContextCreateInfo(createInfo) {
    // Append extensions required by GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    utils::name::appendIfNotPresent(
        extensions, std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount));
    // Append Khronos Debug Utils extension
    if (debugConfig.debugUtilsExtension) {
        utils::name::appendIfNotPresent(extensions, { VK_EXT_DEBUG_UTILS_EXTENSION_NAME });
    }
    // Append LunarG validation layer
    if (debugConfig.validationLayer) {
        utils::name::appendIfNotPresent(layers, { VK_LAY_KHRONOS_VALIDATION_LAYER_NAME });
    }
}

class DefaultPhysicalDevicePicker {
public:
    const vk::raii::PhysicalDevice& operator()(const vk::raii::PhysicalDevices& devices) const {
        return pick(devices);
    }

private:
    using Score = int;

    static const vk::raii::PhysicalDevice& pick(const vk::raii::PhysicalDevices& devices) {
        std::vector<int> scores(devices.size());
        for (std::size_t i = 0; i < devices.size(); i++) {
            scores[i] += getDeviceTypeScore(devices[i].getProperties().deviceType);
            scores[i] += getDeviceMemoryScore(devices[i].getMemoryProperties());
        }
        auto bestDeviceIndex = std::distance(std::ranges::max_element(scores), scores.begin());
        return devices[std::size_t(bestDeviceIndex)];
    }

    static Score getDeviceTypeScore(vk::PhysicalDeviceType type) {
        switch (type) {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return 8;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            return 2;
        case vk::PhysicalDeviceType::eVirtualGpu:
            return 0;
        case vk::PhysicalDeviceType::eCpu:
            return -2;
        case vk::PhysicalDeviceType::eOther:
            return -8;
        }
        throw std::logic_error("Unsupported vk::PhysicalDeviceType");
    }

    static Score getDeviceMemoryScore(const vk::PhysicalDeviceMemoryProperties& memoryProperties) {
        constexpr auto isLocalHeap = [](const vk::MemoryHeap& heap) {
            return bool(heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal);
        };
        auto localHeaps
            = std::span(memoryProperties.memoryHeaps.data(), memoryProperties.memoryHeapCount)
            | std::views::filter(isLocalHeap);
        auto largestHeap = std::ranges::max(localHeaps, {}, &vk::MemoryHeap::size);
        // Score is number of GB of memory
        return Score(largestHeap.size / (1024 * 1024 * 1024));
    }
};

vk::raii::PhysicalDevice Instance::pickPhysicalDevice() const {
    return pickPhysicalDevice(DefaultPhysicalDevicePicker{});
}

bool Instance::isDeviceCompatible(const vk::raii::PhysicalDevice& device) {
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

void Instance::removeIncompatiblePhysicalDevices(vk::raii::PhysicalDevices& devices) const {
    auto incompatibleCount
        = std::erase_if(devices, [](auto&& device) { return !isDeviceCompatible(device); });
    std::cout << "Removed " << incompatibleCount << " devices\n";
}

} // namespace magma
