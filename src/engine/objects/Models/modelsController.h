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

namespace vax::objects {
class ModelsController {
  public:
    friend class ModelLoader;

    explicit ModelsController(
        vax::ResourceManager& resourceManager,
        vax::objects::ModelLoader& modelLoader,
        vax::objects::PrimitivesBuilder& primitivesBuilder
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
        const std::vector<vax::objects::ModelDescriptor>& modelDescriptors,
        vax::vk::CommandBuffer& commandBuffer,
        VkQueue submitQueue
    );

    std::vector<std::string> getModelIds() const;

    std::vector<std::string> getSceneNodeIds() const;

    std::optional<vax::objects::SceneNode> getPreloadedSceneNodeById(const std::string& id, uint32_t instancesCount);

    std::optional<vax::objects::SceneNode>
    createSceneNodeById(const std::string& id, std::vector<vax::math::Transform> transforms = {vax::math::Transform()});

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
    };

    vax::utils::Logger _logger = vax::utils::Logger("ModelsController");
    const uint32_t _maxDrawableInstances = vax::MAX_DRAWABLE_INSTANCES;

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::objects::ModelLoader> _modelLoader;
    std::reference_wrapper<vax::objects::PrimitivesBuilder> _primitivesBuilder;

    std::unordered_map<std::string, vax::objects::SceneNode> _cachedSceneNodeMap;

    std::unordered_map<std::string, ModelInfo> _modelMap;
    std::vector<vax::objects::DrawableModel> _drawableModels;

    uint32_t _globalInstanceCursor = 0;

    DrawableModelHandle
    _addDrawableModel(std::string id, std::string path, vax::objects::DrawableModel&& drawableModel);
};
} // namespace vax::objects