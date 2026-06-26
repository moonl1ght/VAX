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

    void preloadModels(
        const std::vector<vax::objects::ModelDescriptor>& modelDescriptors,
        vax::vk::CommandBuffer& commandBuffer,
        VkQueue submitQueue
    );

    std::vector<std::string> getModelNames() const;

    std::optional<vax::objects::SceneNode> getSceneNode(const std::string& name);

  private:
    struct ModelInfo {
        vax::objects::ModelDescriptor modelDescriptor;
        size_t modelIndex;
    };

    vax::utils::Logger _logger = vax::utils::Logger("ModelsController");
    const uint32_t _maxDrawableInstances = vax::MAX_DRAWABLE_INSTANCES;

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::objects::ModelLoader> _modelLoader;
    std::reference_wrapper<vax::objects::PrimitivesBuilder> _primitivesBuilder;

    std::unordered_map<std::string, ModelInfo> _modelInfos;
    std::vector<vax::objects::DrawableModel> _drawableModels;
};
} // namespace vax::objects