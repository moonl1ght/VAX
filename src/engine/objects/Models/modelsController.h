#pragma once

#include "logger.h"
#include "vkUtils.h"

namespace vax::objects {
class ModelsController {
  public:
    explicit ModelsController() {};

    ~ModelsController() {};

    ModelsController(const ModelsController& other) = delete;
    ModelsController& operator=(const ModelsController& other) = delete;
    ModelsController(ModelsController&& other) noexcept = delete;
    ModelsController& operator=(ModelsController&& other) noexcept = delete;

    bool canAddDrawableInstance() const;

    bool addDrawableInstance(uint32_t instanceCount);

    bool removeDrawableInstance(uint32_t instanceCount);

    uint32_t drawableInstancesCount() const;

    uint32_t maxDrawableInstances() const;

    void resetDrawableInstancesCounter();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("ModelsController");
    const uint32_t _maxDrawableInstances = vax::MAX_DRAWABLE_INSTANCES;

    uint32_t _drawableInstancesCount = 0;

};
} // namespace vax::objects