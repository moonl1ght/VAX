#include "sceneNode.h"
#include "shaderUniforms.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::engine;
using namespace vax;
using namespace vax::math;

void SceneNode::updateInstanceData(
    const DrawContext& drawContext, const InstanceData& instanceData, uint32_t instanceIndex
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
        _ssboManager.get().updateInstance(drawContext.currentFrame, offset + instanceIndex, instanceData);
    }
}

void SceneNode::drawModels(const DrawContext& drawContext) {
    for (size_t i = 0; i < _drawableModels.size(); ++i) {
        auto& drawableModel = _drawableModels[i];
        for (auto& drawingRange : _drawableModelInstanceDrawingRanges[i]) {
            DrawableModel::DrawSettings drawSettings = {
                .instanceOffset = drawingRange.first,
                .instancesCount = drawingRange.second,
            };
            drawableModel->draw(drawContext, drawSettings);
        }
    }
}

void SceneNode::draw(const DrawContext& drawContext) {
    Node::draw(drawContext);
}

void SceneNode::addDrawableModel(DrawableModelHandle drawableModelHandle) {
    _drawableModels.push_back(drawableModelHandle.drawableModel);
    _drawableModelInstanceDrawingRanges.push_back(drawableModelHandle.instanceDrawingRanges);
}