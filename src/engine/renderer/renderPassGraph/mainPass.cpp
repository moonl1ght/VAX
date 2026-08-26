#include "mainPass.h"

using namespace vax::engine;

void MainPass::runPass(RunPassInfo& runPassInfo) {
    _setViewportAndScissor(runPassInfo.commandBuffer, _renderArea.extent);
    _pass(runPassInfo, [&]() {
        auto pipeline = _pipelineManager.get().getPipeline(vax::vk::PipelineName::PBR);
        auto pipelineLayout = _pipelineManager.get().getPipelineLayout(vax::vk::PipelineLayoutName::BASE);
        if (!pipeline) {
            return;
        }
        auto frameDescriptorSetHandler = _descriptorSetManager.get().createDefaultDescriptorSetHandler(
            runPassInfo.frameIndex,
            vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
            vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
            "per_frame",
            false
        );

        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get default descriptor set writer!");
            return;
        }
        uint32_t offset = 0;
        frameDescriptorSetHandler->bind(
            runPassInfo.commandBuffer.vkCommandBuffer,
            pipelineLayout,
            MainSetIndices::PER_FRAME_SET_INDEX,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            1,
            &offset
        );

        if (!runPassInfo.commandBuffer.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS))
            return;
        DrawContext drawContext{
            .commandBuffer = runPassInfo.commandBuffer.vkCommandBuffer,
            .pipelineLayout = pipeline->vkPipelineLayout,
            .currentFrame = runPassInfo.frameIndex,
        };
        runPassInfo.scene->draw(drawContext);

        if (!_drawGizmo(runPassInfo)) {
            _logger.error("Failed to draw gizmo!");
        }
    });
}

bool MainPass::_drawGizmo(RunPassInfo& runPassInfo) {
    VkClearAttachment clearAttachment{
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .clearValue = {.depthStencil = {0.0f, 0}},
    };

    auto xOffset = static_cast<float>(_swapchainExtent.width - 256);

    VkClearRect clearRect{
        .rect = {.offset = {static_cast<int32_t>(xOffset), 0}, .extent = {256, 256}},
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkViewport viewport{
        .x = xOffset,
        .y = 256.0f,
        .width = 256.0f,
        .height = -256.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor{
        .offset = {static_cast<int32_t>(xOffset), 0},
        .extent = {256, 256},
    };

    vkCmdSetViewport(runPassInfo.commandBuffer.vkCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(runPassInfo.commandBuffer.vkCommandBuffer, 0, 1, &scissor);

    vkCmdClearAttachments(runPassInfo.commandBuffer.vkCommandBuffer, 1, &clearAttachment, 1, &clearRect);
    auto gizmoPipeline = _pipelineManager.get().getPipeline(vax::vk::PipelineName::BASE);
    if (!gizmoPipeline)
        return false;
    if (!runPassInfo.commandBuffer.bindPipeline(gizmoPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS))
        return false;
    DrawContext drawContext{
        .commandBuffer = runPassInfo.commandBuffer.vkCommandBuffer,
        .pipelineLayout = gizmoPipeline->vkPipelineLayout,
        .currentFrame = runPassInfo.frameIndex,
    };
    runPassInfo.scene->drawGizmo(drawContext);
    return true;
}