#pragma once

#include <vulkan/vulkan.h>

#include "window_base.hpp"

#include <vector>

template <typename T>
class VIVDevice
{
public:
    VIVDevice(VIVDevice&) = delete;
    VIVDevice& operator=(VIVDevice&) = delete;

    VIVDevice(BaseWindow<T>&);
    ~VIVDevice();

    VkDevice getDevice() const { return _device; }

private:
    void createInstance();
    void pickSuitablePhysicalDevice();
    uint32_t getSuitableQueueFamilyIndex(VkPhysicalDevice);
    void createDevice();
    void createSurface();
    VkInstance _instance;
    VkPhysicalDevice _physical_device;
    VkDevice _device;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    BaseWindow<T>& _window;
};

#include "vivdevice.inl"