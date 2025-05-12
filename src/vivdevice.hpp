#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "window_base.hpp"

#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphics_family_has_value = false;
    bool present_family_has_value = false;
    bool isComplete() { return graphics_family_has_value && present_family_has_value; }
};

struct TextureDescriptor
{
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
    VkDescriptorSet set;
};

template <typename T>
class VIVDevice
{
public:
    VIVDevice(VIVDevice&) = delete;
    VIVDevice& operator=(VIVDevice&) = delete;

    VIVDevice(BaseWindow<T>&);
    ~VIVDevice();

    VkCommandPool getCommandPool() { return _command_pool; }
    VkDevice getDevice() const { return _device; }
    VkSurfaceKHR getSurface() const { return _surface; }
    VkQueue getGraphicsQueue() const { return _graphics_queue; }
    VkQueue getPresentQueue() const { return _present_queue; }
    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(_physical_device); }
    QueueFamilyIndices getPhysicalQueueFamilies() { return getQueueFamilies(_physical_device); }
    VkFormat getSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkDescriptorSet& getDescriptorSet() { return _texture_descriptor.set; };
    VkDescriptorSetLayout& getDescriptorSetLayout() { return _texture_descriptor.layout; };

    void createImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);
    VkImage createTextureImage(const std::string& image_name);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
    VkImageView createImageView(VkImage image, VkFormat format);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    VkSampler createTextureSampler();
    void createTextureSamplerDescriptor();
    void updateDescriptorSetWithImage(VkImageView textureImageView, VkSampler textureSampler);

    void importImage(const std::string& path);

    
private:
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
    void createInstance();
    void pickSuitablePhysicalDevice();
    uint32_t getSuitableQueueFamilyIndex(VkPhysicalDevice);
    void createDevice();
    void createSurface();
    void createCommandPool();

    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    VkImageView createTextureImageView(VkImage textureImage);

    VkInstance _instance;
    VkPhysicalDevice _physical_device;
    VkDevice _device;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    BaseWindow<T>& _window;
    VkCommandPool _command_pool;
    VkQueue _graphics_queue;
    VkQueue _present_queue;
    TextureDescriptor _texture_descriptor{};
};

#include "vivdevice.inl"