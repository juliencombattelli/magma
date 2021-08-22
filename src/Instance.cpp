#include <magma/Instance.hpp>

#include <iostream>

#ifndef VK_LAY_KHRONOS_VALIDATION_LAYER_NAME
#define VK_LAY_KHRONOS_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

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

} // namespace magma
