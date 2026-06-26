#pragma once

#include "drawableModel.h"
#include "logger.h"
#include "modelLoader.h"
#include "resourceManager.h"
#include "vkUtils.h"

namespace vax::objects {
class ModelsController {
  public:
    explicit ModelsController(vax::ResourceManager& resourceManager, vax::objects::ModelLoader& modelLoader)
        : _resourceManager(resourceManager)
        , _modelLoader(modelLoader) {};

    ~ModelsController() {};

    ModelsController(const ModelsController& other) = delete;
    ModelsController& operator=(const ModelsController& other) = delete;
    ModelsController(ModelsController&& other) noexcept = delete;
    ModelsController& operator=(ModelsController&& other) noexcept = delete;

    uint32_t maxDrawableInstances() const { return _maxDrawableInstances; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("ModelsController");
    const uint32_t _maxDrawableInstances = vax::MAX_DRAWABLE_INSTANCES;

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::objects::ModelLoader> _modelLoader;
    std::vector<vax::objects::DrawableModel> _drawableModels;
};
} // namespace vax::objects