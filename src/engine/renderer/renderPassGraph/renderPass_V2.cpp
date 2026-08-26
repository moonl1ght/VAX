#include "renderPass_V2.h"

using namespace vax::engine;

void RenderPass_V2::runPass(RunPassInfo& runPassInfo) {
    if (_prePassWork) {
        _prePassWork(runPassInfo);
    }
    _setViewportAndScissor(runPassInfo.commandBuffer, _renderArea.extent);
    _pass(runPassInfo, [&]() {
        auto pipeline = _pipelineManager.get().getPipeline(_pipelineName);
        auto pipelineLayout = _pipelineManager.get().getPipelineLayout(_pipelineLayoutName);
        if (!pipeline) {
            _logger.error("Failed to get pipeline!");
            return;
        }
        if (!pipelineLayout) {
            _logger.error("Failed to get pipeline layout!");
            return;
        }
        for (const auto& inputDescriptorSetInfo : _inputDescriptorSetInfos) {
            auto descriptorSetHandler = _descriptorSetManager.get().createDefaultDescriptorSetHandler(
                runPassInfo.frameIndex,
                inputDescriptorSetInfo.poolType,
                inputDescriptorSetInfo.layoutName,
                inputDescriptorSetInfo.name,
                true
            );
            if (!descriptorSetHandler.has_value()) {
                _logger.error("Failed to get descriptor set handler!");
                return;
            }
            descriptorSetHandler->bind(
                runPassInfo.commandBuffer.vkCommandBuffer,
                pipelineLayout,
                inputDescriptorSetInfo.bindingInfo.setIndex,
                inputDescriptorSetInfo.bindingInfo.bindPoint,
                inputDescriptorSetInfo.bindingInfo.dynamicOffsetCount,
                inputDescriptorSetInfo.bindingInfo.dynamicOffsets.data()
            );
        }

        if (!runPassInfo.commandBuffer.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS))
            return;
        DrawContext drawContext{
            .commandBuffer = runPassInfo.commandBuffer.vkCommandBuffer,
            .pipelineLayout = pipeline->vkPipelineLayout,
            .currentFrame = runPassInfo.frameIndex,
        };
        if (_drawWork) {
            _drawWork(runPassInfo, drawContext);
        }
    });
    if (_postPassWork) {
        _postPassWork(runPassInfo);
    }
}

void RenderPass_V2::_setViewportAndScissor(vax::vk::CommandBuffer& commandBuffer, VkExtent2D extent) {
    VkViewport viewport{
        .x = 0.0f,
        .y = static_cast<float>(extent.height),
        .width = static_cast<float>(extent.width),
        .height = -static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer.vkCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{.offset = {0, 0}, .extent = extent};
    vkCmdSetScissor(commandBuffer.vkCommandBuffer, 0, 1, &scissor);
}