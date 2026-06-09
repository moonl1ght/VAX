#pragma once

#include "drawableModel.h"
#include "transform.h"
#include <vector>

namespace vax::objects {
class SceneNode final {
  public:
    std::vector<SceneNode> children;
    std::vector<DrawableModel> drawableModels;

    explicit SceneNode(
        std::string name,
        const vax::math::Transform& originalParentRelativeTransform,
        bool isRoot = false
    )
        : _name(std::move(name))
        , _isRoot(isRoot)
        , _originalParentRelativeTransform(originalParentRelativeTransform) {};
    ~SceneNode() = default;

    SceneNode(const SceneNode& other) = default;
    SceneNode& operator=(const SceneNode& other) = default;

    SceneNode(SceneNode&& other) noexcept = default;
    SceneNode& operator=(SceneNode&& other) = default;

    bool isRoot() const { return _isRoot; };
    const std::string& name() const { return _name; };
    bool hasDrawableModels() const { return !drawableModels.empty(); };

    void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

    template <typename T> void updateTransform(const T& updater) {
        updater(_transformHandle);
        _isSelfTransformDirty = true;
        _isParentTransformDirty = true;
    }

    void updateParentTransformMatrices(const glm::mat4& modelMatrix) {
        _parentTransformMatrices.updateModelMatrix(modelMatrix);
    }

  private:
    std::string _name;
    bool _isRoot = false;
    const vax::math::Transform _originalParentRelativeTransform;
    vax::math::TransformMatrixHandle _parentTransformMatrices;
    vax::math::TransformHandle _transformHandle;
    bool _isSelfTransformDirty = false;
    bool _isParentTransformDirty = false;
};
} // namespace vax::objects