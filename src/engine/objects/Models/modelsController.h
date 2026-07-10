#pragma once

#include "commandBuffer.h"
#include "drawableModel.h"
#include "logger.h"
#include "modelDescriptor.h"
#include "modelLoader.h"
#include "primitivesBuilder.h"
#include "resourceManager.h"
#include "sceneNode.h"
#include "vkUtils.h"

namespace vax::engine {
class ModelsController {
  public:
    friend class ModelLoader;

    explicit ModelsController(
        vax::vk::ResourceManager& resourceManager,
        vax::engine::ModelLoader& modelLoader,
        vax::engine::PrimitivesBuilder& primitivesBuilder
    )
        : _resourceManager(resourceManager)
        , _modelLoader(modelLoader)
        , _primitivesBuilder(primitivesBuilder) {
        _drawableModels.reserve(_maxDrawableInstances);
    };

    ~ModelsController() {};

    ModelsController(const ModelsController& other) = delete;
    ModelsController& operator=(const ModelsController& other) = delete;
    ModelsController(ModelsController&& other) noexcept = delete;
    ModelsController& operator=(ModelsController&& other) noexcept = delete;

    uint32_t maxDrawableInstances() const { return _maxDrawableInstances; }

    void preload(
        const std::vector<vax::engine::ModelDescriptor>& modelDescriptors,
        vax::vk::CommandBuffer& commandBuffer,
        VkQueue submitQueue
    );

    std::vector<std::string> getModelIds() const;

    std::vector<std::string> getSceneNodeIds() const;

    std::optional<vax::engine::SceneNode> getPreloadedSceneNodeById(const std::string& id, uint32_t instancesCount);

    std::optional<vax::engine::SceneNode> createSceneNodeById(
        const std::string& id,
        std::vector<vax::math::Transform> transforms = {vax::math::Transform()}
    );

    DrawableModel* getDrawableModelById(const std::string& id);

  private:
    struct ModelInfo final {
        struct SSBOChunkInfo final {
            uint32_t instanceOffset;
            uint32_t cursor;
            uint32_t maxInstances;

            bool isFull() const { return cursor >= maxInstances; }
        };

        size_t modelIndex;
        std::vector<SSBOChunkInfo> ssboChunkInfos;
        uint32_t ssboChunkCursor = 0;
        bool isIdentifiable = true;
    };

    vax::Logger _logger = vax::Logger("ModelsController");
    const uint32_t _maxDrawableInstances = vax::vk::MAX_DRAWABLE_INSTANCES;

    std::reference_wrapper<vax::vk::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::engine::ModelLoader> _modelLoader;
    std::reference_wrapper<vax::engine::PrimitivesBuilder> _primitivesBuilder;

    std::unordered_map<std::string, vax::engine::SceneNode> _cachedSceneNodeMap;

    std::unordered_map<std::string, ModelInfo> _modelMap;
    std::vector<vax::engine::DrawableModel> _drawableModels;

    uint32_t _globalInstanceCursor = 0;
    uint32_t _lastObjectId = 0;

    DrawableModelHandle _addDrawableModel(std::string id, std::string path, vax::engine::DrawableModel&& drawableModel);
};
} // namespace vax::engine