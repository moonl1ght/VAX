#include "sceneNode.h"

using namespace vax::objects;
using namespace vax;

void SceneNode::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    auto worldHandle = _parentTransformMatrices;
    if (_isSelfTransformDirty) {
        worldHandle.updateModelMatrix(
            _parentTransformMatrices.getModelMatrix() * _transformHandle.getModelMatrix()
        );
    }

    for (auto& drawableModel : drawableModels) {
        drawableModel.transformMatrixHandle = worldHandle;
        drawableModel.draw(commandBuffer, pipelineLayout);
    }

    for (auto& child : children) {
        if (_isParentTransformDirty) {
            child._parentTransformMatrices.updateModelMatrix(
                worldHandle.getModelMatrix() * child._originalParentRelativeTransform.getModelMatrix()
            );
            child._isParentTransformDirty = true;
        }
        child.draw(commandBuffer, pipelineLayout);
    }

    _isParentTransformDirty = false;
    _isSelfTransformDirty = false;
}