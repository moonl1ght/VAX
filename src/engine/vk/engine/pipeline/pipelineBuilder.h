#pragma once

#include "luna.h"
#include "pipeline.h"
#include "device.h"

namespace vax::vk {

    // MARK: - PipelineBuilder

    class PipelineBuilder {
    public:
        explicit PipelineBuilder(const vax::vk::Device& device) : _device(device) {};

        virtual ~PipelineBuilder() = default;

        virtual std::optional<vax::vk::Pipeline> build(vax::vk::PipelineName pipelineName) = 0;

    protected:
        std::reference_wrapper<const vax::vk::Device> _device;
    };

    // MARK: - ComputePipelineBuilder

    class ComputePipelineBuilder final : public PipelineBuilder {
    public:

        explicit ComputePipelineBuilder(const vax::vk::Device& device) : PipelineBuilder(device) {};

        ~ComputePipelineBuilder() {
            if (_pipelineLayout != VK_NULL_HANDLE && !_isPipelineLayoutTransferred) {
                vkDestroyPipelineLayout(_device.get().vkDevice, _pipelineLayout, nullptr);
            }
        };

        std::optional<vax::vk::Pipeline> build(vax::vk::PipelineName pipelineName) override;

        bool setPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo);

        bool updatePipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo);

        /// Replace the current shader stage with the new one.
        void setShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("ComputePipelineBuilder");
        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo _shaderStageInfo = {};
        bool _isPipelineLayoutTransferred = false;
    };

    // MARK: - GraphicsPipelineBuilder

    class GraphicsPipelineBuilder final : public PipelineBuilder {
    public:

        explicit GraphicsPipelineBuilder(const vax::vk::Device& device) : PipelineBuilder(device) {};

        std::optional<vax::vk::Pipeline> build(vax::vk::PipelineName pipelineName) override;

        void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);
        void addVertexInputInfo(
            VkVertexInputBindingDescription bindingDescription,
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions
        );
        void setInputAssembly(VkPrimitiveTopology topology) {
            _topology = topology;
        }
        void addDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);
        void setPushConstantRange(VkPushConstantRange pushConstantRange) {
            _pushConstantRange = pushConstantRange;
        }
        void setRenderPass(VkRenderPass renderPass) {
            _renderPass = renderPass;
        }

    private:
        vax::utils::Logger _logger = vax::utils::Logger("GraphicsPipelineBuilder");
        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
        VkVertexInputBindingDescription _bindingDescription = {};
        std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
        std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;
        VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkPushConstantRange _pushConstantRange = {};
        VkRenderPass _renderPass = VK_NULL_HANDLE;
    };
}