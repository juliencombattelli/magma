#include <magma/Instance.hpp>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <ranges>
#include <span>

#ifndef VK_LAY_KHRONOS_VALIDATION_LAYER_NAME
#define VK_LAY_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#endif

static bool debugCallbackCpp(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT /*type*/,
    const vk::DebugUtilsMessengerCallbackDataEXT& cbData, void* /*userdata*/) {

    if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
        spdlog::error(cbData.pMessage);
    } else if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        spdlog::warn(cbData.pMessage);
    } else if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
        spdlog::info(cbData.pMessage);
    } else if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
        spdlog::debug(cbData.pMessage);
    }
    return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* cbData,
    void* userdata) {

    vk::DebugUtilsMessengerCallbackDataEXT callbackData;
    callbackData = *cbData;
    return debugCallbackCpp(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity),
        static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type), callbackData, userdata);
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
            spdlog::error("Extension {} not supported", requestedExtension);
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
            spdlog::error("Layer {} not supported", requestedLayer);
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
            spdlog::set_level(spdlog::level::debug);
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

vk::raii::PhysicalDevice Instance::pickPhysicalDevice(const vk::raii::SurfaceKHR& surface) const {
    return pickPhysicalDevice(surface, DefaultPhysicalDevicePicker{});
}

static bool areRequiredDeviceExtensionsAvailable(const vk::raii::PhysicalDevice& device) {
    static const std::vector<std::string_view> requiredExtensions
        = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const auto availableExtensions = device.enumerateDeviceExtensionProperties();
    bool extensionsSupported = true;
    // Check availability of all extensions
    for (auto extension : requiredExtensions) {
        if (!utils::contains(
                availableExtensions, extension, &vk::ExtensionProperties::extensionName)) {
            spdlog::debug("Device does not support extension {}", extension);
            extensionsSupported = false;
        }
    }
    return extensionsSupported;
}

static bool isAtLeastOneSurfaceFormatAvailable(
    const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
    auto surfaceFormats = device.getSurfaceFormatsKHR(*surface);
    if (surfaceFormats.empty()) {
        spdlog::debug("Device does not provide any surface format");
        return false;
    }
    return true;
}

static bool isAtLeastOneSurfacePresentModeAvailable(
    const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
    auto surfacePresentModes = device.getSurfacePresentModesKHR(*surface);
    if (surfacePresentModes.empty()) {
        spdlog::debug("Device does not provide any surface present mode");
        return false;
    }
    return true;
}

static bool areGraphicsAndPresentationCapabilitiesSupported(
    const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
    auto queueFamilyProperties = device.getQueueFamilyProperties();
    std::optional<uint32_t> queueGraphicsFamilyIndex;
    for (auto queueFamilyIt = queueFamilyProperties.begin();
         queueFamilyIt != queueFamilyProperties.end(); ++queueFamilyIt) {
        if (queueFamilyIt->queueFlags & vk::QueueFlagBits::eGraphics) {
            queueGraphicsFamilyIndex = std::distance(queueFamilyProperties.begin(), queueFamilyIt);
        }
        if (!queueGraphicsFamilyIndex) {
            spdlog::debug("Device does not support graphics");
            return false;
        }
        auto presentSupport = device.getSurfaceSupportKHR(*queueGraphicsFamilyIndex, *surface);
        if (!presentSupport) {
            spdlog::debug("Device does not support presentation");
            return false;
        }
    }
    return true;
}

bool Instance::isDeviceCompatible(
    const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {

    const auto& deviceName = device.getProperties().deviceName;

    spdlog::debug("Checking if physical device {} is compatible", deviceName);

    constexpr auto fails = [](bool test) { return test == false; };
    auto checks = {
        areRequiredDeviceExtensionsAvailable(device),
        isAtLeastOneSurfaceFormatAvailable(device, surface),
        isAtLeastOneSurfacePresentModeAvailable(device, surface),
        areGraphicsAndPresentationCapabilitiesSupported(device, surface),
    };
    if (std::ranges::any_of(checks, fails)) {
        spdlog::warn("Physical device {} is not compatible", deviceName);
        return false;
    }

    spdlog::debug("Physical device {} is compatible", deviceName);
    return true;
}

void Instance::removeIncompatiblePhysicalDevices(
    vk::raii::PhysicalDevices& devices, const vk::raii::SurfaceKHR& surface) const {
    auto incompatibleCount = std::erase_if(
        devices, [&surface](auto&& device) { return !isDeviceCompatible(device, surface); });
    spdlog::debug("Removed {} incompatible physical devices", incompatibleCount);
}

} // namespace magma
