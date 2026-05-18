#include "drawableModel.h"

using namespace vax::objects;


bool DrawableModel::loadMesh(
    vax::vk::CommandBuffer& commandBuffer
) {
    return _mesh->loadBuffers(commandBuffer);
}

void DrawableModel::draw(
    vax::vk::Engine* vkEngine,
    VkCommandBuffer commandBuffer,
    VkPipelineLayout pipelineLayout,
    float time
) {
    if (!_mesh->isLoaded()) return;
    VkBuffer vertexBuffers[] = { _mesh->vertexBuffer->vkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _mesh->indexBuffer->vkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    DrawPushConstants drawPushConstants{};
    drawPushConstants.model = transform.getModelMatrix();
    drawPushConstants.normalMatrix = transform.getNormalMatrix();

    uint32_t flags = ObjectFlags::NoFlags;
    if (_settings.useWireframe) {
        flags |= ObjectFlags::IsWireframe;
    }
    if (!_settings.hasTangents) {
        flags |= ObjectFlags::NoTangent;
    }

    drawPushConstants.flags = flags;
    bool skipPushConstants = _settings.skipPushConstants;
    for (auto& submesh : _submeshes) {
        if (!skipPushConstants) {
            drawPushConstants.materialIndex = submesh.materialIndex;
            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DrawPushConstants),
                &drawPushConstants
            );
        }

        vkCmdDrawIndexed(
            commandBuffer,
            submesh.indexCount,
            1,
            submesh.firstIndex,
            submesh.vertexOffset,
            0
        );
    }
}