#include "sceneNode.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace vax::objects;
using namespace vax;
using namespace vax::math;

void SceneNode::draw(const vax::renderer::DrawContext& drawContext) {
    TransformMatrixHandle worldHandle;
    // TODO: cache this matrix

    worldHandle.updateModelMatrix(_parentTransformMatrices.getModelMatrix() * _transformHandle.getModelMatrix());

    for (auto& drawableModel : _drawableModels) {
        drawableModel->updateSSBO(drawContext.currentFrame, {worldHandle});
        drawableModel->draw(drawContext);
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

void SceneNode::addDrawableModel(DrawableModel* drawableModel) {
    _drawableModels.push_back(drawableModel);
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