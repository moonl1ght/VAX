#include "drawableNode.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::engine;
using namespace vax;
using namespace vax::math;

void DrawableNode::updateInstanceData(
    uint32_t frameIndex, const InstanceData& instanceData, uint32_t instanceIndex
) {
    for (auto& drawingRangeForDrawableModel : _drawableModelInstanceDrawingRanges) {
        auto drawingRange = drawingRangeForDrawableModel[_drawingRangeIndex];
        auto count = drawingRange.second;
        // if the instance index is greater than the count (batch size), we need to increment the drawing range
        // index
        while (instanceIndex >= count) {
            ++_drawingRangeIndex;
            drawingRange = drawingRangeForDrawableModel[_drawingRangeIndex];
            count = drawingRange.second;
        }
        auto offset = drawingRange.first;
        _ssboManager.get().updateInstance(frameIndex, offset + instanceIndex, instanceData);
    }
}

void DrawableNode::prepareDrawingModels(engine::IndirectDrawController* indirectDrawController, uint32_t frameIndex) {
    for (size_t i = 0; i < _drawableModels.size(); ++i) {
        auto& drawableModel = _drawableModels[i];
        for (auto& drawingRange : _drawableModelInstanceDrawingRanges[i]) {
            DrawableModel::DrawSettings drawSettings = {
                .instanceOffset = drawingRange.first,
                .instancesCount = drawingRange.second,
            };
            drawableModel->prepareDrawing(indirectDrawController, frameIndex, drawSettings);
        }
    }
}

void DrawableNode::prepareDrawing(engine::IndirectDrawController* indirectDrawController, uint32_t frameIndex) {
    Node::prepareDrawing(indirectDrawController, frameIndex);
}

void DrawableNode::addDrawableModel(DrawableModelHandle drawableModelHandle) {
    _drawableModels.push_back(drawableModelHandle.drawableModel);
    _drawableModelInstanceDrawingRanges.push_back(drawableModelHandle.instanceDrawingRanges);
}