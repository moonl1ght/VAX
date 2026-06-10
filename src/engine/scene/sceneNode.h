#pragma once

#include "drawableModel.h"
#include "transform.h"
#include <optional>
#include <vector>

namespace vax::objects {
class ModelLoader;
}

namespace vax::objects {
class SceneNode final {
  public:
    friend class vax::objects::ModelLoader;

    explicit SceneNode(
        std::string name,
        const vax::math::Transform& originalParentRelativeTransform,
        vax::math::TransformMatrixHandle parentTransformMatrices,
        bool isRoot = false
    )
        : _name(std::move(name))
        , _isRoot(isRoot)
        , _parentTransformMatrices(parentTransformMatrices)
        , _originalParentRelativeTransform(originalParentRelativeTransform) {};

    ~SceneNode() = default;

    SceneNode(const SceneNode& other) = delete;
    SceneNode& operator=(const SceneNode& other) = delete;

    SceneNode(SceneNode&& other) noexcept = default;
    SceneNode& operator=(SceneNode&& other) = default;

    bool isRoot() const { return _isRoot; };
    const std::string& name() const { return _name; };
    bool hasDrawableModels() const { return !_drawableModels.empty(); };

    void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

    /// @param depth - depth of the child, if -1 search from all children with max depth,
    ///                 if 0 will compare only with node itself.
    SceneNode* getChild(const std::string& name, int depth = -1);

    template <typename T> void updateTransform(const T& updater) {
        updater(_transformHandle);
        _isSelfTransformDirty = true;
        _isChildrenTransformDirty = true;
    }

    void insertChild(SceneNode&& child);

    void insertDrawableModel(DrawableModel&& drawableModel);

    const std::vector<SceneNode>& children() const { return _children; }
    const std::vector<DrawableModel>& drawableModels() const { return _drawableModels; }

    void loadDrawableModelsMeshes(vax::vk::CommandBuffer& commandBuffer);

  private:
    std::string _name;
    std::vector<SceneNode> _children;
    std::vector<DrawableModel> _drawableModels;
    bool _isRoot = false;
    const vax::math::Transform _originalParentRelativeTransform;
    vax::math::TransformMatrixHandle _parentTransformMatrices;
    vax::math::TransformHandle _transformHandle;
    bool _isSelfTransformDirty = false;
    bool _isChildrenTransformDirty = false;
};
} // namespace vax::objects