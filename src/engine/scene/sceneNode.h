#pragma once

#include "drawContext.h"
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

    void draw(const vax::renderer::DrawContext& drawContext);

    /// @param depth - depth of the child, if -1 search from all children with max depth,
    ///                 if 0 will compare only with node itself.
    SceneNode* getChild(const std::string& name, int depth = -1);

    /// Accumulates transform
    template <typename T> void updateTransform(const T& updater) {
        updater(_transformHandle);
        _isChildrenTransformDirty = true;
    }

    void insertChild(SceneNode&& child);

    // TODO: move to private
    void addDrawableModel(DrawableModel* drawableModel);

    // TODO: remove this one
    void insertDrawableModel(DrawableModel&& drawableModel);

    const std::vector<SceneNode>& children() const { return _children; }

    const std::vector<DrawableModel*>& drawableModelsConst() const { return _drawableModels; }

    std::vector<DrawableModel*> drawableModels() { return _drawableModels; }

    // TODO: remove this one
    void loadDrawableModelsMeshes(const vax::objects::MeshPBR::LoadMeshBuffersContext& context);

    const vax::math::TransformHandle& transformHandle() const { return _transformHandle; }

  private:
    std::string _name;
    std::vector<SceneNode> _children;
    std::vector<DrawableModel*> _drawableModels;
    bool _isRoot = false;
    vax::math::Transform _originalParentRelativeTransform;
    vax::math::TransformMatrixHandle _parentTransformMatrices;
    vax::math::TransformHandle _transformHandle;
    bool _isChildrenTransformDirty = false;
};
} // namespace vax::objects