#include "drawableModel.h"

using namespace vax::objects;

bool DrawableModel::loadMesh(const MeshPBR::LoadMeshBuffersContext& context) { return _mesh->loadBuffers(context); }

void DrawableModel::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    if (!_mesh->isLoaded())
        return;
    VkBuffer vertexBuffers[] = {_mesh->vertexBuffer->vkBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _mesh->indexBuffer->vkBuffer(), 0, VK_INDEX_TYPE_UINT32);

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

    if (_settings.instanceDrawing) {
        _drawInstance(commandBuffer, pipelineLayout, flags);
    } else {
        _drawSingleMesh(commandBuffer, pipelineLayout, flags);
    }
}

void DrawableModel::_drawInstance(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t flags) {
    DrawInstancePushConstants drawPushConstants{};
    drawPushConstants.flags = flags;

    for (auto& submesh : _submeshes) {
        if (!_settings.skipPushConstants) {
            drawPushConstants.materialIndex = submesh.materialIndex;
            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DrawInstancePushConstants),
                &drawPushConstants
            );
        }
        vkCmdDrawIndexed(commandBuffer, submesh.indexCount, _settings.instancesCount, submesh.firstIndex, submesh.vertexOffset, 0);
    }
}

void DrawableModel::_drawSingleMesh(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t flags) {
    DrawPushConstants drawPushConstants{};
    if (transformHandle.has_value()) {
        drawPushConstants.model = transformHandle->getModelMatrix();
        drawPushConstants.normalMatrix = transformHandle->getNormalMatrix();
    } else {
        drawPushConstants.model = instanceTransformMatrixHandles[0].getModelMatrix();
        drawPushConstants.normalMatrix = instanceTransformMatrixHandles[0].getNormalMatrix();
    }
    drawPushConstants.flags = flags;

    for (auto& submesh : _submeshes) {
        if (!_settings.skipPushConstants) {
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
        vkCmdDrawIndexed(commandBuffer, submesh.indexCount, 1, submesh.firstIndex, submesh.vertexOffset, 0);
    }
}