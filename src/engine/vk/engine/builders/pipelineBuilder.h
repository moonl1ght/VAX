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

        virtual std::optional<std::unique_ptr<vax::vk::Pipeline>> build() = 0;

    protected:
        std::reference_wrapper<const vax::vk::Device> _device;
    };

    // MARK: - ComputePipelineBuilder

    class ComputePipelineBuilder final : public PipelineBuilder {
    public:

        explicit ComputePipelineBuilder(const vax::vk::Device& device) : PipelineBuilder(device) {};

        ~ComputePipelineBuilder() {
            if (pipelineLayout != VK_NULL_HANDLE && !isPipelineLayoutTransferred) {
                vkDestroyPipelineLayout(_device.get().vkDevice, pipelineLayout, nullptr);
            }
        };

        std::optional<std::unique_ptr<vax::vk::Pipeline>> build() override;

        bool setPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo);

        bool updatePipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo);

        /// Replace the current shader stage with the new one.
        void setShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);

    private:
        Logger _logger = Logger("ComputePipelineBuilder");
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo shaderStageInfo = {};
        bool isPipelineLayoutTransferred = false;
    };

    // MARK: - GraphicsPipelineBuilder

    class GraphicsPipelineBuilder final : public PipelineBuilder {
    public:

        explicit GraphicsPipelineBuilder(const vax::vk::Device& device) : PipelineBuilder(device) {};

        std::optional<std::unique_ptr<vax::vk::Pipeline>> build() override;

        void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);

    private:
        Logger _logger = Logger("GraphicsPipelineBuilder");
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    };
}