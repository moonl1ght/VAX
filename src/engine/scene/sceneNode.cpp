#include "sceneNode.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::objects;
using namespace vax;
using namespace vax::math;

void SceneNode::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    TransformMatrixHandle worldHandle;
    // TODO: cache this matrix
    worldHandle.updateModelMatrix(
        _parentTransformMatrices.getModelMatrix() * _transformHandle.getModelMatrix()
    );

    for (auto& drawableModel : _drawableModels) {
        drawableModel.instanceTransformMatrixHandles[0] = worldHandle;
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
}

void SceneNode::insertChild(SceneNode&& child) {
    _children.push_back(std::move(child));
    _isChildrenTransformDirty = true;
}

void SceneNode::insertDrawableModel(DrawableModel&& drawableModel) {
    _drawableModels.push_back(std::move(drawableModel));
}

void SceneNode::loadDrawableModelsMeshes(const vax::objects::MeshPBR::LoadMeshBuffersContext& context) {
    for (auto& drawableModel : _drawableModels) {
        drawableModel.loadMesh(context);
    }
    for (auto& child : _children) {
        child.loadDrawableModelsMeshes(context);
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