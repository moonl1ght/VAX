#include "pipelineBuilder.h"

using namespace vax::vk;

// MARK: - ComputePipelineBuilder

std::optional<std::unique_ptr<Pipeline>> ComputePipelineBuilder::build() {
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
        _device.get().vkDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &pipeline
    );
    if (!VK_CHECK(result)) {
        _logger.error("Failed to create compute pipeline!");
        return std::nullopt;
    }

    isPipelineLayoutTransferred = true;
    return std::make_optional<std::unique_ptr<vax::vk::Pipeline>>(
        std::make_unique<vax::vk::Pipeline>(
            _device.get(), vax::vk::PipelineType::COMPUTE, pipelineLayout, pipeline
        )
    );
}

bool ComputePipelineBuilder::setPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
    if (pipelineLayout != VK_NULL_HANDLE) {
        _logger.warning("Pipeline layout already set!");
        return false;
    }
    auto pipelineLayoutResult = vkCreatePipelineLayout(
        _device.get().vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout
    );
    if (!VK_CHECK(pipelineLayoutResult)) {
        _logger.error("Failed to create pipeline layout!");
        return false;
    }
    return true;
}

bool ComputePipelineBuilder::updatePipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device.get().vkDevice, pipelineLayout, nullptr);
    }
    auto pipelineLayoutResult = vkCreatePipelineLayout(
        _device.get().vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout
    );
    if (!VK_CHECK(pipelineLayoutResult)) {
        _logger.error("Failed to create pipeline layout!");
        return false;
    }
    return true;
}

void ComputePipelineBuilder::setShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name) {
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = module;
    shaderStageInfo.pName = name;
    this->shaderStageInfo = shaderStageInfo;
}

// MARK: - GraphicsPipelineBuilder

std::optional<std::unique_ptr<vax::vk::Pipeline>> GraphicsPipelineBuilder::build() {
    return std::nullopt;
}

void GraphicsPipelineBuilder::addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name) {
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = module;
    shaderStageInfo.pName = name;
    shaderStages.push_back(shaderStageInfo);
}