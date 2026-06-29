#include "sceneNode.h"
#include "shaderUniforms.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::objects;
using namespace vax;
using namespace vax::math;

void SceneNode::draw(const vax::renderer::DrawContext& drawContext) {
    TransformMatrixHandle worldHandle;
    // TODO: cache this matrix

    worldHandle.updateModelMatrix(_parentTransformMatrices.getModelMatrix() * _transformHandle.getModelMatrix());

    for (auto& drawableModelHandle : _drawableModels) {
        // InstanceData instanceData = {
        //     .model = worldHandle.getModelMatrix(),
        //     .normalMatrix = worldHandle.getNormalMatrix(),
        // };
        // auto index = drawableModelHandle.instanceOffset + drawableModelHandle.instancesCount;
        // _ssboManager.get().updateInstance(drawContext.currentFrame, index, instanceData);
        drawableModelHandle.drawableModel->draw(
            drawContext, drawableModelHandle.instanceOffset, drawableModelHandle.instancesCount
        );
    }

    for (auto& child : _children) {
        if (_isChildrenTransformDirty) {
            child._parentTransformMatrices.updateModelMatrix(
                worldHandle.getModelMatrix() * child._originalParentRelativeTransform.getModelMatrix()
            );
            child._isChildrenTransformDirty = true;
        }
        child.draw(drawContext);
    }

    _isChildrenTransformDirty = false;
}

void SceneNode::insertChild(SceneNode&& child) {
    _children.push_back(std::move(child));
    _isChildrenTransformDirty = true;
}

void SceneNode::addDrawableModel(DrawableModelHandle drawableModelHandle) {
    _drawableModels.push_back(drawableModelHandle);
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