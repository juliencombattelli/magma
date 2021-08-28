#pragma once

namespace magma {

template<typename TPhysicalDevicePicker>
vk::raii::PhysicalDevice Instance::pickPhysicalDevice(TPhysicalDevicePicker&& pick) const {
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

} // namespace magma