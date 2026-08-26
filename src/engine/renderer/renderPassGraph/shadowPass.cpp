#include "shadowPass.h"

using namespace vax::engine;

void ShadowPass::runPass(RunPassInfo& runPassInfo) {
    _setViewportAndScissor(runPassInfo.commandBuffer, _renderArea.extent);
    _pass(runPassInfo, [&]() {
        auto pipeline = _pipelineManager.get().getPipeline(vax::vk::PipelineName::SHADOW);
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
        uint32_t offset = static_cast<uint32_t>(1 * runPassInfo.scene->passUboStride());
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
    });
}