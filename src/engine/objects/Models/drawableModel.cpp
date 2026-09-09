#include "drawableModel.h"

using namespace vax::engine;
using namespace vax::vk;

void DrawableModel::draw(const DrawContext& drawContext, const DrawSettings& drawSettings) {
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

    // DrawPushConstants drawPushConstants{};
    // drawPushConstants.flags = flags;

    for (auto& submesh : _submeshes) {
        // if (!_settings.skipPushConstants) {
        //     drawPushConstants.materialIndex = submesh.materialIndex;
        //     vkCmdPushConstants(
        //         drawContext.commandBuffer.vkCommandBuffer,
        //         drawContext.pipelineLayout,
        //         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        //         0,
        //         sizeof(DrawPushConstants),
        //         &drawPushConstants
        //     );
        // }
        if (drawContext.indirectDrawController) {
            VkDrawIndexedIndirectCommand drawIndexedIndirectCommand{
                .indexCount = submesh.indexCount,
                .instanceCount = drawSettings.instancesCount,
                .firstIndex = _mesh->globalMemoryIndexCursor().offset + submesh.firstIndex,
                .vertexOffset = static_cast<int32_t>(_mesh->globalMemoryVertexCursor().offset + submesh.vertexOffset),
                .firstInstance = drawSettings.instanceOffset,
            };
            PerDrawData perDrawData{
                .flags = flags,
                .materialIndex = submesh.materialIndex,
            };
            drawContext.indirectDrawController->pushCommand(drawIndexedIndirectCommand, perDrawData);
        } else {
            vkCmdDrawIndexed(
                drawContext.commandBuffer.vkCommandBuffer,
                submesh.indexCount,
                drawSettings.instancesCount,
                _mesh->globalMemoryIndexCursor().offset + submesh.firstIndex,
                _mesh->globalMemoryVertexCursor().offset + submesh.vertexOffset,
                drawSettings.instanceOffset
            );
        }
    }
}