#include "sceneNode.h"
#include "shaderUniforms.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::engine;
using namespace vax;
using namespace vax::math;

void SceneNode::draw(const DrawContext& drawContext) {
    // TODO: update only dirty instances
    size_t drawingRangeIndex = 0;
    for (size_t i = 0; i < _instancesCount; ++i) {
        TransformMatrixHandle worldHandle;
        auto& transformInfo = _drawableModelTransformInfos[i];
        worldHandle.updateModelMatrix(
            transformInfo._parentTransformMatrices.getModelMatrix() * transformInfo._transformHandle.getModelMatrix()
        );
        InstanceData instanceData = {
            .model = worldHandle.getModelMatrix(),
            .normalMatrix = worldHandle.getNormalMatrix(),
            .flags = static_cast<uint32_t>(
                _isSelected ? InstanceFlags::IsInstanceSelected : InstanceFlags::InstanceFlagsNone
            ),
            .instanceId = _nodeId == NO_ID ? NO_ID : static_cast<uint32_t>(_nodeId + i),
        };
        for (auto& drawingRangeForDrawableModel : _drawableModelInstanceDrawingRanges) {
            auto& drawingRange = drawingRangeForDrawableModel[drawingRangeIndex];
            auto count = drawingRange.second;
            // if the instance index is greater than the count (batch size), we need to increment the drawing range
            // index
            while (i >= count) {
                ++drawingRangeIndex;
                drawingRange = drawingRangeForDrawableModel[drawingRangeIndex];
                count = drawingRange.second;
            }
            auto offset = drawingRange.first;
            _ssboManager.get().updateInstance(drawContext.currentFrame, offset + i, instanceData);
        }

        for (auto& child : _children) {
            if (transformInfo._isChildrenTransformDirty) {
                const size_t chunkSize = child._instancesCount / _instancesCount;
                for (size_t j = 0; j < chunkSize; ++j) {
                    auto& childTransformInfo = child._drawableModelTransformInfos[i * chunkSize + j];
                    childTransformInfo._parentTransformMatrices.updateModelMatrix(
                        worldHandle.getModelMatrix() *
                        childTransformInfo._originalParentRelativeTransform.getModelMatrix()
                    );
                    childTransformInfo._isChildrenTransformDirty = true;
                }
            }
        }
        _drawableModelTransformInfos[i]._isChildrenTransformDirty = false;
    }

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

    for (auto& child : _children) {
        child.draw(drawContext);
    }
}

void SceneNode::insertChild(SceneNode&& child) {
    _children.push_back(std::move(child));
    for (size_t i = 0; i < _instancesCount; ++i) {
        _drawableModelTransformInfos[i]._isChildrenTransformDirty = true;
    }
}

void SceneNode::addDrawableModel(DrawableModelHandle drawableModelHandle) {
    _drawableModels.push_back(drawableModelHandle.drawableModel);
    _drawableModelInstanceDrawingRanges.push_back(drawableModelHandle.instanceDrawingRanges);
}

SceneNode* SceneNode::getChild(const std::string& name, int depth) {
    if (name == _name) {
        return this;
    }
    if (depth == 0) {
        return nullptr;
    }
    for (auto& child : _children) {
        auto result = child.getChild(name, depth == -1 ? depth : depth - 1);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void SceneNode::setNodeId(uint32_t nodeId, bool propagate) {
    _nodeId = nodeId;
    if (propagate) {
        for (auto& child : _children) {
            child.setNodeId(nodeId, true);
        }
    }
}

void SceneNode::setIsSelected(bool isSelected, bool propagate) {
    _isSelected = isSelected;
    if (propagate) {
        for (auto& child : _children) {
            child.setIsSelected(isSelected, true);
        }
    }
}