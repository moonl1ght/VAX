#include "sceneNode.h"

using namespace vax::objects;
using namespace vax;

void SceneNode::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    auto worldHandle = _parentTransformMatrices;
    if (_isSelfTransformDirty) {
        worldHandle.updateModelMatrix(_parentTransformMatrices.getModelMatrix() * _transformHandle.getModelMatrix());
    }

    for (auto& drawableModel : _drawableModels) {
        drawableModel.transformMatrixHandle = worldHandle;
        drawableModel.draw(commandBuffer, pipelineLayout);
    }

    for (auto& child : _children) {
        if (_isChildrenTransformDirty) {
            child._parentTransformMatrices.updateModelMatrix(
                worldHandle.getModelMatrix() * child._originalParentRelativeTransform.getModelMatrix()
            );
            child._isChildrenTransformDirty = true;
        }
        child.draw(commandBuffer, pipelineLayout);
    }

    _isChildrenTransformDirty = false;
    _isSelfTransformDirty = false;
}

void SceneNode::insertChild(SceneNode&& child) {
    _children.push_back(std::move(child));
    _isChildrenTransformDirty = true;
}

void SceneNode::insertDrawableModel(DrawableModel&& drawableModel) {
    _drawableModels.push_back(std::move(drawableModel));
}

void SceneNode::loadDrawableModelsMeshes(vax::vk::CommandBuffer& commandBuffer) {
    for (auto& drawableModel : _drawableModels) {
        drawableModel.loadMesh(commandBuffer);
    }
    for (auto& child : _children) {
        child.loadDrawableModelsMeshes(commandBuffer);
    }
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