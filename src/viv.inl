#include "viv.hpp"


template <typename T>
VulkanImageViewer<T>::VulkanImageViewer(int w, int h, const std::string& app_name)
{
    _window = std::make_unique<T>(w, h, app_name.c_str());
    _device.emplace(*_window);
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
}

template <typename T>
VulkanImageViewer<T>::~VulkanImageViewer()
{

}

template <typename T>
void VulkanImageViewer<T>::createPipelineLayout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(glm::mat4) * 2;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_device->getDescriptorSetLayout();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &push_constant_range;
    if(vkCreatePipelineLayout(_device->getDevice(), &pipelineLayoutInfo, nullptr, &_pipeline_layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout");
}


template <typename T>
void VulkanImageViewer<T>::createPipeline()
{
    auto pipelineConfig = VIVPipeline<T>::defaultPipelineConfigInfo(_swapchain->width(), _swapchain->height());
    pipelineConfig.renderPass = _swapchain->getRenderPass();
    pipelineConfig.pipelineLayout = _pipeline_layout;
    _pipeline.emplace(*_device, SHADERS_DIR "first.vert.spir", SHADERS_DIR "first.fragment.spir", pipelineConfig);
}


template <typename T>
void VulkanImageViewer<T>::recreateSwapChain()
{
    auto extent = _window->getExtent();
    vkDeviceWaitIdle(_device->getDevice());
    _swapchain.emplace(*_device, extent);
    createPipeline();
}


template <typename T>
void VulkanImageViewer<T>::createCommandBuffers()
{
    _command_buffers.resize(_swapchain->imageCount());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _device->getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(_command_buffers.size());

    if(vkAllocateCommandBuffers(_device->getDevice(), &allocInfo, _command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

template <typename T>
void VulkanImageViewer<T>::drawFrame()
{
    uint32_t imageIndex;
    auto result = _swapchain->acquireNextImage(&imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire next swap chain image.");
    }

    recordCommandBuffer(imageIndex);
    result = _swapchain->submitCommandBuffers(&_command_buffers[imageIndex], &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window->wasWindowResized())
    {
        _window->resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image");
    }

    _window->drawOverlay();

}

template <typename T>
void VulkanImageViewer<T>::recordCommandBuffer(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if(vkBeginCommandBuffer(_command_buffers[imageIndex], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _swapchain->getRenderPass();
    renderPassInfo.framebuffer = _swapchain->getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapchain->getSwapChainExtent();
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(_command_buffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    _pipeline->bind(_command_buffers[imageIndex]);

    vkCmdBindDescriptorSets(
        _command_buffers[imageIndex],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _pipeline_layout,
        0,
        1,
        &_device->getDescriptorSet(),
        0,
        nullptr
    );

    vkCmdPushConstants(
        _command_buffers[imageIndex],
        _pipeline_layout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        128,
        &_window->getView()
    );
    
    vkCmdDraw(_command_buffers[imageIndex], 6, 1, 0, 0);
    vkCmdEndRenderPass(_command_buffers[imageIndex]);
    if(vkEndCommandBuffer(_command_buffers[imageIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer.");
    }
}