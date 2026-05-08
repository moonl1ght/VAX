#include "drawableModel.h"

using namespace vax::objects;


bool DrawableModel::loadMesh(
    VkQueue submitQueue,
    vax::vk::CommandBuffer& commandBuffer
) {
    return _mesh->loadBuffers(submitQueue, commandBuffer);
}

void DrawableModel::draw(
    vax::vk::Engine* vkEngine,
    VkCommandBuffer commandBuffer,
    const vax::vk::PipelineManager& pipelineManager,
    float time
) {
    if (!_mesh->isLoaded()) return;
    VkBuffer vertexBuffers[] = { _mesh->vertexBuffer.value().vkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _mesh->indexBuffer.value().vkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    for (auto& submesh : _submeshes) {
        DrawPushConstants drawPushConstants{};
        drawPushConstants.model = transform.getModelMatrix();
        drawPushConstants.flags = ObjectFlags::NoFlags;
        drawPushConstants.materialIndex = submesh.materialIndex;
        
        vkCmdPushConstants(
            commandBuffer,
            pipelineManager.getPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(DrawPushConstants),
            &drawPushConstants
        );

        vkCmdDrawIndexed(
            commandBuffer,
            submesh.indexCount,
            1,
            submesh.firstIndex,
            submesh.vertexOffset,
            0
        );
    }
    // _mesh->forceDraw(*vkEngine->queueManager, *vkEngine->commandManager, commandBuffer);
}