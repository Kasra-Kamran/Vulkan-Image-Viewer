#include "vivdevice.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

#include <cassert>

using namespace std;

template <typename T>
VIVDevice<T>::VIVDevice(BaseWindow<T>& window)
    : _window(window)
{
    createInstance();
    createSurface();
    pickSuitablePhysicalDevice();
    createDevice();
}

template <typename T>
VIVDevice<T>::~VIVDevice()
{
    vkDestroyDevice(_device, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

template <typename T>
void VIVDevice<T>::createInstance()
{
    auto e = BaseWindow<T>::requiredExtensions();
    vector<const char*> extensions;
    for(auto& ext : e)
    {
        extensions.push_back(ext.c_str());
    }
    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;


    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_create_info.ppEnabledExtensionNames = extensions.data();
    instance_create_info.enabledLayerCount = 0;

    if(vkCreateInstance(&instance_create_info, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to creat vulkan instance.");
    }
}

template <typename T>
void VIVDevice<T>::pickSuitablePhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(_instance, &device_count, nullptr);
    if(device_count == 0)
    {
        throw std::runtime_error("No physical devices found.");
    }

    vector<VkPhysicalDevice> physical_devices(device_count);
    vkEnumeratePhysicalDevices(_instance, &device_count, physical_devices.data());
    int queue_family_index = -1;
    for(int i = 0; i < physical_devices.size(); i++)
    {
        if(getSuitableQueueFamilyIndex(physical_devices[i]) != -1)
        {
            _physical_device = physical_devices[i];
            return;
        }
    }

    throw runtime_error("Found no device capable of graphics stuff.");

    
}

template <typename T>
uint32_t VIVDevice<T>::getSuitableQueueFamilyIndex(VkPhysicalDevice physical_device)
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    
    vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties.data());
    for(int j = 0; j < queue_family_properties.size(); j++)
    {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, j, _surface, &present_support);
        if(queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && present_support)
        {
            return j;
        }
    }
    return -1;
}

template <typename T>
void VIVDevice<T>::createDevice()
{
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = getSuitableQueueFamilyIndex(_physical_device);
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.enabledLayerCount = 0;

    if (vkCreateDevice(_physical_device, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }
}

template <typename T>
void VIVDevice<T>::createSurface()
{
    _window.createWindowSurface(_instance, _surface);
}