#pragma once

#include "drawContext.h"
#include "drawableModel.h"
#include "ssboManager.h"
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

    struct DrawableModelTransformInfo final {
        vax::math::Transform _originalParentRelativeTransform;
        vax::math::TransformMatrixHandle _parentTransformMatrices;
        vax::math::TransformHandle _transformHandle;
        bool _isChildrenTransformDirty = false;
    };

    explicit SceneNode(
        vax::vk::SSBOManager& ssboManager,
        std::string name,
        const vax::math::Transform& originalParentRelativeTransform,
        vax::math::TransformMatrixHandle parentTransformMatrices,
        bool isRoot = false,
        uint32_t instancesCount = 1
    )
        : _ssboManager(ssboManager)
        , _name(std::move(name))
        , _instancesCount(instancesCount)
        , _isRoot(isRoot) {
        _drawableModelTransformInfos.reserve(instancesCount);
        for (uint32_t i = 0; i < instancesCount; ++i) {
            _drawableModelTransformInfos.push_back({
                ._originalParentRelativeTransform = originalParentRelativeTransform,
                ._parentTransformMatrices = parentTransformMatrices,
                ._transformHandle = vax::math::TransformHandle(),
                ._isChildrenTransformDirty = false,
            });
        }
    };

    SceneNode(
        vax::vk::SSBOManager& ssboManager,
        std::string name,
        std::vector<vax::math::Transform> transforms,
        bool isRoot = false
    )
        : _ssboManager(ssboManager)
        , _name(std::move(name))
        , _instancesCount(transforms.size())
        , _isRoot(isRoot) {
        _drawableModelTransformInfos.reserve(transforms.size());
        for (size_t i = 0; i < transforms.size(); ++i) {
            vax::math::TransformHandle transformHandle;
            transformHandle.setTransform(transforms[i]);
            _drawableModelTransformInfos.push_back({
                ._originalParentRelativeTransform = vax::math::Transform(),
                ._parentTransformMatrices = vax::math::TransformMatrixHandle(),
                ._transformHandle = std::move(transformHandle),
                ._isChildrenTransformDirty = false,
            });
        }
    };

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
    template <typename T> void updateTransform(const T& updater, uint32_t instanceIndex = 0) {
        if (instanceIndex >= _instancesCount) {
            return;
        }
        updater(_drawableModelTransformInfos[instanceIndex]._transformHandle);
        _drawableModelTransformInfos[instanceIndex]._isChildrenTransformDirty = true;
    }

    void insertChild(SceneNode&& child);

    const std::vector<SceneNode>& children() const { return _children; }

    const std::vector<DrawableModel*>& drawableModelsConst() const { return _drawableModels; }

    std::vector<DrawableModel*>& drawableModels() { return _drawableModels; }

    void addDrawableModel(DrawableModelHandle drawableModelHandle);

    uint32_t instancesCount() const { return _instancesCount; }

  private:
    std::reference_wrapper<vax::vk::SSBOManager> _ssboManager;

    std::string _name;
    uint32_t _instancesCount;
    std::vector<SceneNode> _children;
    std::vector<DrawableModel*> _drawableModels;
    std::vector<std::vector<DrawableModelHandle::InstanceDrawingRange>> _drawableModelInstanceDrawingRanges;
    bool _isRoot = false;
    std::vector<DrawableModelTransformInfo> _drawableModelTransformInfos;
};
} // namespace vax::objects