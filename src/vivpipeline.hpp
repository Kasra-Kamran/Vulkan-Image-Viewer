#pragma once

#include "vivdevice.hpp"

#include <string>
#include <vector>



struct PipelineConfigInfo
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};
template <typename T>
class VIVPipeline
{
    public:
    VIVPipeline(
        VIVDevice<T>& device,
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath,
            const PipelineConfigInfo& config);
        ~VIVPipeline();
        VIVPipeline(const VIVPipeline&) = delete;
        VIVPipeline& operator=(const VIVPipeline&) = delete;
        void bind(VkCommandBuffer commandBuffer);
        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
    private:
        static std::vector<char> readFile(const std::string& filePath);
        void createGraphicsPipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const PipelineConfigInfo& config);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        VIVDevice<T>& _device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
};

#include "vivpipeline.inl"