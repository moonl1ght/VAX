#include "drawableModel.h"

using namespace vax::objects;
using namespace vax::renderer;

bool DrawableModel::loadMesh(const MeshPBR::LoadMeshBuffersContext& context) { return _mesh->loadBuffers(context); }

void DrawableModel::draw(const DrawContext& drawContext) {
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

    if (_settings.instanceDrawing) {
        _drawInstance(drawContext, flags);
    } else {
        _drawSingleMesh(drawContext, flags);
    }
    if (drawContext.descriptorHandler) {
        if (_descriptorSetId == vax::NullId) {
            _descriptorSetId = drawContext.descriptorHandler->id();
            std::cout << "descriptorSetId: " << _descriptorSetId << std::endl;
        }
    }
}

void DrawableModel::_drawInstance(const DrawContext& drawContext, uint32_t flags) {
    DrawInstancePushConstants drawPushConstants{};
    drawPushConstants.flags = flags;

    for (auto& submesh : _submeshes) {
        if (!_settings.skipPushConstants) {
            drawPushConstants.materialIndex = submesh.materialIndex;
            vkCmdPushConstants(
                drawContext.commandBuffer,
                drawContext.pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DrawInstancePushConstants),
                &drawPushConstants
            );
        }
        vkCmdDrawIndexed(drawContext.commandBuffer, submesh.indexCount, _settings.instancesCount, submesh.firstIndex, submesh.vertexOffset, 0);
    }
}

void DrawableModel::_drawSingleMesh(const DrawContext& drawContext, uint32_t flags) {
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
                drawContext.commandBuffer,
                drawContext.pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DrawPushConstants),
                &drawPushConstants
            );
        }
        vkCmdDrawIndexed(drawContext.commandBuffer, submesh.indexCount, 1, submesh.firstIndex, submesh.vertexOffset, 0);
    }
}