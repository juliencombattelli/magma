#pragma once

#include <magma/Vulkan.hpp>

namespace magma {

template<typename TPhysicalDevicePicker>
vk::raii::PhysicalDevice Instance::pickPhysicalDevice(
    const vk::raii::SurfaceKHR& surface,
    TPhysicalDevicePicker&& pick) const
{
    vk::raii::PhysicalDevices devices(instance_);
    removeIncompatiblePhysicalDevices(devices, surface);
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

} // namespace magma
