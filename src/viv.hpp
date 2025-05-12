#pragma once


#include <vector>
#include <optional>
#include <memory>
#include <string>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "window_base.hpp"
#include "vivdevice.hpp"
#include "vivpipeline.hpp"
#include "vivswapchain.hpp"
#include "window_qt.hpp"


template <typename T>
class VulkanImageViewer
{
public:
    VulkanImageViewer(int w, int h, const std::string& app_name);
    ~VulkanImageViewer();
    VulkanImageViewer(const VulkanImageViewer&) = delete;
    VulkanImageViewer& operator=(const VulkanImageViewer&) = delete;
    void importImage(const std::string& path) { _device->importImage(path); }
    void drawFrame();
    void recordCommandBuffer(uint32_t imageIndex);
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void recreateSwapChain();
    std::unique_ptr<BaseWindow<T>>& getWindow() { return _window; } 
    private:
    std::optional<VIVDevice<T>> _device;
    VkPipelineLayout _pipeline_layout;
    std::optional<VIVPipeline<T>> _pipeline;
    std::optional<VIVSwapChain<T>> _swapchain;
    std::vector<VkCommandBuffer> _command_buffers;
    std::unique_ptr<BaseWindow<T>> _window;
};

#include "viv.inl"

extern template class VulkanImageViewer<VIVqtWindow>;