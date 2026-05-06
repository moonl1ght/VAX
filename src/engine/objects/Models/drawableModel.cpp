#include "drawableModel.h"

using namespace vax::objects;


bool DrawableModel::loadMesh(
    vax::vk::QueueManager& queueManager,
    vax::vk::CommandManager& commandManager
) {
    return _mesh->loadBuffers(queueManager, commandManager);
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
        // drawPushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
        // drawPushConstants.model = transform.getModelMatrix();
        drawPushConstants.model = glm::rotate(
            transform.getModelMatrix(), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f)
        );
        drawPushConstants.flags = ObjectFlags::None;

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