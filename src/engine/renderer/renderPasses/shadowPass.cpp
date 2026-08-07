#include "shadowPass.h"

using namespace vax::engine;

void ShadowPass::runPass(RunPassInfo& runPassInfo) {
    _setViewportAndScissor(runPassInfo.commandBuffer, runPassInfo.renderArea.extent);
    _pass(runPassInfo, [&]() {
        auto pipeline = _pipelineManager.get().getPipeline(vax::vk::PipelineName::SHADOW);
        if (!pipeline) {
            return;
        }
        // auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
        //     _currentFrame, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "per_frame", "per_frame", false
        // );

        // if (!frameDescriptorSetHandler.has_value()) {
        //     _logger.error("Failed to get default descriptor set writer!");
        //     return;
        // }
        // uint32_t offset = static_cast<uint32_t>(1 * renderPassInfo.scene->passUboStride());
        // frameDescriptorSetHandler->bind(
        //     renderPassInfo.commandBuffer->vkCommandBuffer,
        //     renderPassInfo.pipelineLayout,
        //     MainSetIndices::PER_FRAME_SET_INDEX,
        //     VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     1,
        //     &offset
        // );

        // if (!renderPassInfo.commandBuffer->bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS))
        //     return;
        // DrawContext drawContext{
        //     .commandBuffer = renderPassInfo.commandBuffer->vkCommandBuffer,
        //     .pipelineLayout = pipeline->vkPipelineLayout,
        //     .currentFrame = _currentFrame,
        // };
        // renderPassInfo.scene->draw(drawContext);
    });
}