#include "drawableModel.h"

using namespace vax::engine;
using namespace vax::vk;

bool DrawableModel::loadMesh(const MeshPBR::LoadMeshBuffersContext& context) { return _mesh->loadBuffers(context); }

void DrawableModel::draw(const DrawContext& drawContext, const DrawSettings& drawSettings) {
    if (!_mesh->isLoaded())
        return;
    VkBuffer vertexBuffers[] = {_mesh->vertexBuffer->vkBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(drawContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(drawContext.commandBuffer, _mesh->indexBuffer->vkBuffer(), 0, VK_INDEX_TYPE_UINT32);

    uint32_t flags = ObjectFlags::NoFlags;
    if (_settings.useWireframe) {
        flags |= ObjectFlags::IsWireframe;
    }
    if (!_settings.hasTangents) {
        flags |= ObjectFlags::NoTangent;
    }
    if (_settings.precomputedMVP) {
        flags |= ObjectFlags::PrecomputedMVP;
    }

    if (drawSettings.isObjectSelected) {
        flags |= ObjectFlags::IsObjectSelected;
    }

    DrawPushConstants drawPushConstants{};
    drawPushConstants.flags = flags;
    drawPushConstants.objectId = drawSettings.objectId;

    for (auto& submesh : _submeshes) {
        if (!_settings.skipPushConstants) {
            drawPushConstants.materialIndex = submesh.materialIndex;
            vkCmdPushConstants(
                drawContext.commandBuffer,
                drawContext.pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DrawPushConstants),
                &drawPushConstants
            );
        }
        vkCmdDrawIndexed(
            drawContext.commandBuffer,
            submesh.indexCount,
            drawSettings.instancesCount,
            submesh.firstIndex,
            submesh.vertexOffset,
            drawSettings.instanceOffset
        );
    }
}