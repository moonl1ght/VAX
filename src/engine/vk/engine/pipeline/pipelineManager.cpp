#include "pipelineManager.h"
#include "descriptorSetManager.h"
#include "pipelineBuilder.h"
#include "shaderModuleBuilder.h"
#include "shaderUniforms.h"
#include "vkEngine.h"

using namespace vax::vk;
using namespace vax;

bool PipelineManager::setup(
    const RenderPassDescriptor& renderPassDescriptor, const RenderPassDescriptor& postProcessRenderPassDescriptor
) {
    if (!_createBackgroundPipelineLayout(PipelineLayoutName::BACKGROUND)) {
        _logger.error("Failed to create background pipeline layout!");
        return false;
    }
    if (!_createBasePipelineLayout(PipelineLayoutName::BASE)) {
        _logger.error("Failed to create base pipeline layout!");
        return false;
    }
    if (!_createPostProcessPipelineLayout(PipelineLayoutName::POST_PROCESS)) {
        _logger.error("Failed to create post process pipeline layout!");
        return false;
    }
    if (!_createPBRPipeline(renderPassDescriptor)) {
        _logger.error("Failed to create PBR pipeline!");
        return false;
    }
    if (!_createBasePipeline(renderPassDescriptor)) {
        _logger.error("Failed to create base pipeline!");
        return false;
    }
    if (!_createBackgroundPipeline(renderPassDescriptor)) {
        _logger.error("Failed to create background pipeline!");
        return false;
    }
    if (!_createPostProcessPipeline(postProcessRenderPassDescriptor)) {
        _logger.error("Failed to create post process pipeline!");
        return false;
    }
    return true;
}

const Pipeline* PipelineManager::getPipeline(PipelineName pipelineName) const {
    auto name = Pipeline::pipelineNameToString(pipelineName);
    auto it = _pipelines.find(name);
    if (it == _pipelines.end()) {
        _logger.error("Pipeline not found!");
        return nullptr;
    }
    return &it->second;
}

bool PipelineManager::_createBackgroundPipeline(const RenderPassDescriptor& renderPassDescriptor) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/background.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/background.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector =
        std::vector<VkVertexInputAttributeDescription>(attributeDescriptions.begin(), attributeDescriptions.end());
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    pipelineBuilder.setRenderPass(renderPassDescriptor.getVkRenderPass());
    pipelineBuilder.setDepthStencilState({
        .depthWriteEnable = false,
        .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    });
    auto name = vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::BACKGROUND);
    auto pipelineLayoutName = vax::vk::Pipeline::pipelineLayoutNameToString(vax::vk::PipelineLayoutName::BACKGROUND);
    auto it = _pipelineLayouts.find(pipelineLayoutName);
    if (it == _pipelineLayouts.end()) {
        _logger.error("Pipeline layout not found!");
        return false;
    }
    auto pipeline = pipelineBuilder.build(name, it->second);
    if (!pipeline) {
        _logger.error("Failed to create background pipeline!");
        return false;
    }
    _pipelines.emplace(
        vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::BACKGROUND), std::move(*pipeline)
    );
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}

bool PipelineManager::_createPostProcessPipeline(const RenderPassDescriptor& renderPassDescriptor) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/background.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/postprocess.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector =
        std::vector<VkVertexInputAttributeDescription>(attributeDescriptions.begin(), attributeDescriptions.end());
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    pipelineBuilder.setRenderPass(renderPassDescriptor.getVkRenderPass());
    pipelineBuilder.setDepthStencilState({
        .depthWriteEnable = false,
        .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    });
    auto name = vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::POST_PROCESS);
    auto pipelineLayoutName = vax::vk::Pipeline::pipelineLayoutNameToString(vax::vk::PipelineLayoutName::POST_PROCESS);
    auto it = _pipelineLayouts.find(pipelineLayoutName);
    if (it == _pipelineLayouts.end()) {
        _logger.error("Pipeline layout not found!");
        return false;
    }
    auto pipeline = pipelineBuilder.build(name, it->second);
    if (!pipeline) {
        _logger.error("Failed to create post process pipeline!");
        return false;
    }
    _pipelines.emplace(
        vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::POST_PROCESS), std::move(*pipeline)
    );
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}

bool PipelineManager::_createPBRPipeline(const RenderPassDescriptor& renderPassDescriptor) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/base.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/pbr.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector =
        std::vector<VkVertexInputAttributeDescription>(attributeDescriptions.begin(), attributeDescriptions.end());
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    pipelineBuilder.setRenderPass(renderPassDescriptor.getVkRenderPass());
    auto name = vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::PBR);
    auto pipelineLayoutName = vax::vk::Pipeline::pipelineLayoutNameToString(vax::vk::PipelineLayoutName::BASE);
    auto it = _pipelineLayouts.find(pipelineLayoutName);
    if (it == _pipelineLayouts.end()) {
        _logger.error("Pipeline layout not found!");
        return false;
    }
    auto pipeline = pipelineBuilder.build(name, it->second);
    if (!pipeline) {
        _logger.error("Failed to create PBR pipeline!");
        return false;
    }
    _pipelines.emplace(vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::PBR), std::move(*pipeline));
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}

bool PipelineManager::_createBasePipeline(const RenderPassDescriptor& renderPassDescriptor) {
    auto vertShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/base.vert.spv"));

    auto fragShaderModule = _shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/base.frag.spv"));
    if (!vertShaderModule || !fragShaderModule) {
        _logger.error("Failed to build shader module!");
        return false;
    }

    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule.value(), "main");
    pipelineBuilder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule.value(), "main");
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto attributeDescriptionsVector =
        std::vector<VkVertexInputAttributeDescription>(attributeDescriptions.begin(), attributeDescriptions.end());
    pipelineBuilder.addVertexInputInfo(bindingDescription, attributeDescriptionsVector);
    pipelineBuilder.setRenderPass(renderPassDescriptor.getVkRenderPass());
    auto name = vax::vk::Pipeline::pipelineNameToString(vax::vk::PipelineName::BASE);
    auto pipelineLayoutName = vax::vk::Pipeline::pipelineLayoutNameToString(vax::vk::PipelineLayoutName::BASE);
    auto it = _pipelineLayouts.find(pipelineLayoutName);
    if (it == _pipelineLayouts.end()) {
        _logger.error("Pipeline layout not found!");
        return false;
    }
    auto pipeline = pipelineBuilder.build(name, it->second);
    if (!pipeline) {
        _logger.error("Failed to create base pipeline!");
        return false;
    }
    _pipelines.emplace(name, std::move(*pipeline));
    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    return true;
}

bool vax::vk::PipelineManager::_createBackgroundPipelineLayout(vax::vk::PipelineLayoutName pipelineLayoutName) {
    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    auto name = vax::vk::Pipeline::pipelineLayoutNameToString(pipelineLayoutName);
    auto pipelineLayout = pipelineBuilder.buildPipelineLayout(name);
    if (!pipelineLayout) {
        _logger.error("Failed to create background pipeline layout!");
        return false;
    }
    _pipelineLayouts.emplace(name, pipelineLayout);
    return true;
}

bool vax::vk::PipelineManager::_createPostProcessPipelineLayout(vax::vk::PipelineLayoutName pipelineLayoutName) {
    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    auto name = vax::vk::Pipeline::pipelineLayoutNameToString(pipelineLayoutName);
    auto postProcessDescriptorSetLayout =
        _descriptorSetManager.get().getDescriptorSetLayout(DescriptorSetLayout::SetType::POST_PROCESS);
    if (!postProcessDescriptorSetLayout) {
        _logger.error("Failed to get post process descriptor set layout!");
        return false;
    }
    pipelineBuilder.addDescriptorSetLayout(postProcessDescriptorSetLayout->getVkDescriptorSetLayout());
    auto pipelineLayout = pipelineBuilder.buildPipelineLayout(name);
    if (!pipelineLayout) {
        _logger.error("Failed to create post process pipeline layout!");
        return false;
    }
    _pipelineLayouts.emplace(name, pipelineLayout);
    return true;
}

bool vax::vk::PipelineManager::_createBasePipelineLayout(vax::vk::PipelineLayoutName pipelineLayoutName) {
    auto pipelineBuilder = vax::vk::GraphicsPipelineBuilder(_device.get());
    auto name = vax::vk::Pipeline::pipelineLayoutNameToString(pipelineLayoutName);
    auto globalDescriptorSetLayout =
        _descriptorSetManager.get().getDescriptorSetLayout(DescriptorSetLayout::SetType::GLOBAL);
    if (!globalDescriptorSetLayout) {
        _logger.error("Failed to get global descriptor set layout!");
        return false;
    }
    pipelineBuilder.addDescriptorSetLayout(globalDescriptorSetLayout->getVkDescriptorSetLayout());
    auto perFrameDescriptorSetLayout =
        _descriptorSetManager.get().getDescriptorSetLayout(DescriptorSetLayout::SetType::PER_FRAME);
    if (!perFrameDescriptorSetLayout) {
        _logger.error("Failed to get per frame descriptor set layout!");
        return false;
    }
    pipelineBuilder.addDescriptorSetLayout(perFrameDescriptorSetLayout->getVkDescriptorSetLayout());
    pipelineBuilder.setPushConstantRange({
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(DrawPushConstants),
    });
    auto pipelineLayout = pipelineBuilder.buildPipelineLayout(name);
    if (!pipelineLayout) {
        _logger.error("Failed to create base pipeline layout!");
        return false;
    }
    _pipelineLayouts.emplace(name, pipelineLayout);
    return true;
}

VkPipelineLayout vax::vk::PipelineManager::getPipelineLayout(vax::vk::PipelineLayoutName pipelineLayoutName) const {
    auto name = vax::vk::Pipeline::pipelineLayoutNameToString(pipelineLayoutName);
    auto it = _pipelineLayouts.find(name);
    if (it == _pipelineLayouts.end()) {
        _logger.error("Pipeline layout not found!");
        return VK_NULL_HANDLE;
    }
    return it->second;
}