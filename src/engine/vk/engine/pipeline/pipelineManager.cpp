#include "pipelineManager.h"
#include "pipelineBuilder.h"
#include "shaderModuleBuilder.h"
#include "descriptorSetManager.h"
#include "vkEngine.h"
#include "shaderUniforms.h"

using namespace vax::vk;
using namespace vax;

bool PipelineManager::setup(const vax::vk::RenderPass& renderPass) {
    if (!_createPBRPipeline(renderPass)) {
        _logger.error("Failed to create PBR pipeline!");
        return false;
    }
    if (!_createBackgroundPipeline(renderPass)) {
        _logger.error("Failed to create background pipeline!");
        return false;
    }
    return true;
}

const vax::vk::Pipeline* vax::vk::PipelineManager::getPipeline(vax::vk::PipelineName pipelineName) const {
    if (_pipelines.find(vax::vk::Pipeline::pipelineNameToString(pipelineName)) == _pipelines.end()) {
        _logger.error("Pipeline not found!");
        return nullptr;
    }
    return &_pipelines.at(vax::vk::Pipeline::pipelineNameToString(pipelineName));
}

bool vax::vk::PipelineManager::_createBackgroundPipeline(const vax::vk::RenderPass& renderPass) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/background.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/background.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = vax::objects::Vertex::getBindingDescription();
    auto attributeDescriptions = vax::objects::Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector = std::vector<VkVertexInputAttributeDescription>(
        attributeDescriptions.begin(), attributeDescriptions.end()
    );
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    pipelineBuilder.setRenderPass(renderPass.getVkRenderPass());
    pipelineBuilder.setDepthStencilState({
        .depthWriteEnable = false,
        .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    });
    auto pipeline = pipelineBuilder.build(vax::vk::PipelineName::BACKGROUND);
    if (!pipeline) {
        _logger.error("Failed to create background pipeline!");
        return false;
    }
    _pipelines.emplace(vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::BACKGROUND), std::move(*pipeline));
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}

bool vax::vk::PipelineManager::_createPBRPipeline(const vax::vk::RenderPass& renderPass) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/base.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/pbr.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = vax::objects::Vertex::getBindingDescription();
    auto attributeDescriptions = vax::objects::Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector = std::vector<VkVertexInputAttributeDescription>(
        attributeDescriptions.begin(), attributeDescriptions.end()
    );
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    auto globalDescriptorSetLayout = _descriptorSetManager.get().getDescriptorSetLayout(
        DescriptorSetLayout::SetType::GLOBAL
    );
    if (!globalDescriptorSetLayout) {
        _logger.error("Failed to get global descriptor set layout!");
        return false;
    }
    pipelineBuilder.addDescriptorSetLayout(globalDescriptorSetLayout->getVkDescriptorSetLayout());
    auto perFrameDescriptorSetLayout = _descriptorSetManager.get().getDescriptorSetLayout(
        DescriptorSetLayout::SetType::PER_FRAME
    );
    if (!perFrameDescriptorSetLayout) {
        _logger.error("Failed to get per frame descriptor set layout!");
        return false;
    }
    pipelineBuilder.addDescriptorSetLayout(perFrameDescriptorSetLayout->getVkDescriptorSetLayout());
    pipelineBuilder.setPushConstantRange({
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(DrawPushConstants)
    });
    pipelineBuilder.setRenderPass(renderPass.getVkRenderPass());
    auto pipeline = pipelineBuilder.build(vax::vk::PipelineName::PBR);
    if (!pipeline) {
        _logger.error("Failed to create PBR pipeline!");
        return false;
    }
    _pipelines.emplace(vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::PBR), std::move(*pipeline));
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}