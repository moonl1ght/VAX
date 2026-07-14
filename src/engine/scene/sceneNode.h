#pragma once

#include "drawContext.h"
#include "drawableModel.h"
#include "modelDescriptor.h"
#include "ssboManager.h"
#include "transform.h"
#include <optional>
#include <stop_token>
#include <unordered_map>
#include <variant>
#include <vector>
#include <colorPalette.h>

namespace vax::engine {
class ModelLoader;
}

namespace vax::engine {
class SceneNode final {
  public:
    friend class vax::engine::ModelLoader;

    using MetadataValue = std::variant<std::string, float, int, bool>;


    struct InstanceInfo final {
        vax::math::Transform _originalParentRelativeTransform;
        vax::math::TransformMatrixHandle _parentTransformMatrices;
        vax::math::TransformHandle _transformHandle;
        uint32_t selectionPackedColor = 0;
        bool isChildrenTransformDirty = false;
        bool isSelected = false;
    };

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
        _instanceInfos.reserve(instancesCount);
        for (uint32_t i = 0; i < instancesCount; ++i) {
            _instanceInfos.push_back({
                ._originalParentRelativeTransform = originalParentRelativeTransform,
                ._parentTransformMatrices = parentTransformMatrices,
                ._transformHandle = vax::math::TransformHandle(),
                .isChildrenTransformDirty = false,
                .isSelected = false,
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
        _instanceInfos.reserve(transforms.size());
        for (size_t i = 0; i < transforms.size(); ++i) {
            vax::math::TransformHandle transformHandle;
            transformHandle.setTransform(transforms[i]);
            _instanceInfos.push_back({
                ._originalParentRelativeTransform = vax::math::Transform(),
                ._parentTransformMatrices = vax::math::TransformMatrixHandle(),
                ._transformHandle = std::move(transformHandle),
                .isChildrenTransformDirty = false,
                .isSelected = false,
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

    void draw(const vax::engine::DrawContext& drawContext);

    /// @param depth - depth of the child, if -1 search from all children with max depth,
    ///                 if 0 will compare only with node itself.
    SceneNode* getChild(const std::string& name, int depth = -1);

    /// Accumulates transform
    template <typename T> void updateTransform(const T& updater, uint32_t instanceIndex = 0) {
        if (instanceIndex >= _instancesCount) {
            return;
        }
        updater(_instanceInfos[instanceIndex]._transformHandle);
        _instanceInfos[instanceIndex].isChildrenTransformDirty = true;
    }

    template <typename T> void updateTransforms(const T& updater) {
        for (uint32_t i = 0; i < _instancesCount; ++i) {
            updater(i, _instanceInfos[i]._transformHandle);
            _instanceInfos[i].isChildrenTransformDirty = true;
        }
    }

    const vax::math::Transform& getTransform(uint32_t instanceIndex = 0) const {
        return _instanceInfos[instanceIndex]._transformHandle.getTransform();
    }

    void insertChild(SceneNode&& child);

    const std::vector<SceneNode>& children() const { return _children; }

    const std::vector<DrawableModel*>& drawableModelsConst() const { return _drawableModels; }

    std::vector<DrawableModel*>& drawableModels() { return _drawableModels; }

    void addDrawableModel(DrawableModelHandle drawableModelHandle);

    uint32_t instancesCount() const { return _instancesCount; }

    void setMetadata(const std::string& key, const MetadataValue& value) { _metadata[key] = value; }

    const std::optional<MetadataValue> getMetadata(const std::string& key) const {
        if (auto it = _metadata.find(key); it != _metadata.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    uint32_t nodeId() const { return _nodeId; }

    void setNodeId(uint32_t nodeId, bool propagate = true);

    bool isSelected() const { return _isSelected; }

    void setIsSelected(bool isSelected, bool propagate = true);

    void setNodeSelectionColor(Color color, bool propagate = true);

    void selectInstance(uint32_t instanceIndex);

    void setSelectionColor(uint32_t instanceIndex, Color color);

  private:
    std::reference_wrapper<vax::vk::SSBOManager> _ssboManager;

    std::string _name;
    std::vector<SceneNode> _children;
    std::unordered_map<std::string, MetadataValue> _metadata;
    std::vector<DrawableModel*> _drawableModels;
    std::vector<std::vector<DrawableModelHandle::InstanceDrawingRange>> _drawableModelInstanceDrawingRanges;
    std::vector<InstanceInfo> _instanceInfos;

    uint32_t _nodeId = NO_ID;
    uint32_t _instancesCount;
    bool _isRoot = false;
    bool _isSelected = false;
};
} // namespace vax::engine