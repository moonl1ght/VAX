#pragma once

#include "luna.h"
#include "pipeline.h"

namespace vax {

    // MARK: - PipelineBuilder

    class PipelineBuilder: public vax::VkObject {
    public:
        PipelineBuilder(vax::VkEngine* vkEngine) : vax::VkObject(vkEngine) {};

        virtual ~PipelineBuilder() = default;

        virtual std::optional<Pipeline> build() = 0;
    };

    // MARK: - ComputePipelineBuilder

    class ComputePipelineBuilder final : public PipelineBuilder {
    public:

        ComputePipelineBuilder(vax::VkEngine* vkEngine) : PipelineBuilder(vkEngine) {};

        ~ComputePipelineBuilder() {
            if (pipelineLayout != VK_NULL_HANDLE && !isPipelineLayoutTransferred) {
                vkDestroyPipelineLayout(vkEngine->device->vkDevice, pipelineLayout, nullptr);
            }
        };

        std::optional<vax::Pipeline> build() override {
            if (shaderStageInfo.stage != VK_SHADER_STAGE_COMPUTE_BIT) {
                _logger.error("Compute pipeline requires compute shader stage!");
                return std::nullopt;
            }

            VkComputePipelineCreateInfo computePipelineCreateInfo{};
            computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            computePipelineCreateInfo.pNext = nullptr;
            computePipelineCreateInfo.layout = pipelineLayout;
            computePipelineCreateInfo.stage = shaderStageInfo;

            VkPipeline pipeline;
            auto result = vkCreateComputePipelines(
                vkEngine->device->vkDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline
            );
            if (!VK_CHECK(result)) {
                _logger.error("Failed to create compute pipeline!");
                return std::nullopt;
            }

            isPipelineLayoutTransferred = true;
            return std::make_optional(vax::Pipeline(vkEngine, vax::PipelineType::COMPUTE, pipelineLayout, pipeline));
        }

        bool setPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
            if (pipelineLayout != VK_NULL_HANDLE) {
                _logger.warning("Pipeline layout already set!");
                return false;
            }
            auto pipelineLayoutResult = vkCreatePipelineLayout(
                vkEngine->device->vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout
            );
            if (!VK_CHECK(pipelineLayoutResult)) {
                _logger.error("Failed to create pipeline layout!");
                return false;
            }
            return true;
        }

        bool updatePipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
            if (pipelineLayout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(vkEngine->device->vkDevice, pipelineLayout, nullptr);
            }
            auto pipelineLayoutResult = vkCreatePipelineLayout(
                vkEngine->device->vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout
            );
            if (!VK_CHECK(pipelineLayoutResult)) {
                _logger.error("Failed to create pipeline layout!");
                return false;
            }
            return true;
        }

        /// Replace the current shader stage with the new one.
        void setShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name) {
            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = stage;
            shaderStageInfo.module = module;
            shaderStageInfo.pName = name;
            this->shaderStageInfo = shaderStageInfo;
        }

    private:
        Logger _logger = Logger("ComputePipelineBuilder");
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo shaderStageInfo = {};
        bool isPipelineLayoutTransferred = false;
    };

    // MARK: - GraphicsPipelineBuilder

    class GraphicsPipelineBuilder final : public PipelineBuilder {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        std::optional<vax::Pipeline> build() override {
            return std::nullopt;
        }

        void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name) {
            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = stage;
            shaderStageInfo.module = module;
            shaderStageInfo.pName = name;
            shaderStages.push_back(shaderStageInfo);
        }

    private:
        Logger _logger = Logger("GraphicsPipelineBuilder");
    };
}