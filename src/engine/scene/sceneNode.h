#pragma once

#include "drawContext.h"
#include "drawableModel.h"
#include "node.h"
#include "ssboManager.h"
#include "transform.h"
#include <colorPalette.h>
#include <vector>

namespace vax::engine {
class ModelLoader;
}

namespace vax::engine {
class SceneNode final : public Node {
  public:
    friend class vax::engine::ModelLoader;

    explicit SceneNode(
        vax::vk::SSBOManager& ssboManager,
        std::string name,
        const vax::math::Transform& originalParentRelativeTransform,
        vax::math::TransformMatrixHandle parentTransformMatrices,
        bool isRoot = false,
        uint32_t instancesCount = 1
    )
        : Node(NodeType::SCENE, std::move(name), instancesCount, isRoot)
        , _ssboManager(ssboManager) {
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
        : Node(NodeType::SCENE, std::move(name), transforms.size(), isRoot)
        , _ssboManager(ssboManager) {
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

    SceneNode(const SceneNode& other) = delete;
    SceneNode& operator=(const SceneNode& other) = delete;

    SceneNode(SceneNode&& other) noexcept = default;
    SceneNode& operator=(SceneNode&& other) = default;

    bool hasDrawableModels() const { return !_drawableModels.empty(); };

    void draw(const vax::engine::DrawContext& drawContext);

    void drawModels(const vax::engine::DrawContext& drawContext);

    const std::vector<DrawableModel*>& drawableModels() const { return _drawableModels; }

    std::vector<DrawableModel*>& drawableModels() { return _drawableModels; }

    void addDrawableModel(DrawableModelHandle drawableModelHandle);

    void resetDrawingRangeIndex() { _drawingRangeIndex = 0; }

    void updateInstanceData(const DrawContext& drawContext,const InstanceData& instanceData, uint32_t instanceIndex);

  private:
    std::reference_wrapper<vax::vk::SSBOManager> _ssboManager;

    std::vector<DrawableModel*> _drawableModels;
    std::vector<std::vector<DrawableModelHandle::InstanceDrawingRange>> _drawableModelInstanceDrawingRanges;
    size_t _drawingRangeIndex = 0;
};
} // namespace vax::engine