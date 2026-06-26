#pragma once

#include "colorPalette.h"
#include "drawableModel.h"
#include "luna.h"
#include "materialManager.h"
#include "meshManager.h"
#include "ssboManager.h"

namespace vax::vk {
class CommandManager;
class QueueManager;
} // namespace vax::vk

namespace vax::objects {
class PrimitivesBuilder {
  public:
    explicit PrimitivesBuilder(
        vax::MeshManager& meshManager,
        vax::SSBOManager& ssboManager,
        vax::MaterialManager& materialManager,
        vax::vk::CommandManager& commandManager,
        vax::vk::QueueManager& queueManager
    )
        : _meshManager(meshManager)
        , _ssboManager(ssboManager)
        , _materialManager(materialManager)
        , _commandManager(commandManager)
        , _queueManager(queueManager) {};

    ~PrimitivesBuilder() {};

    PrimitivesBuilder(const PrimitivesBuilder& other) = delete;
    PrimitivesBuilder(PrimitivesBuilder&& other) noexcept = delete;
    PrimitivesBuilder& operator=(const PrimitivesBuilder& other) = delete;
    PrimitivesBuilder& operator=(PrimitivesBuilder&& other) noexcept = delete;

    std::optional<vax::objects::DrawableModel> createCube(float size, vax::Color color);
    std::optional<vax::objects::DrawableModel> createPlane();

  private:
    std::reference_wrapper<vax::SSBOManager> _ssboManager;
    std::reference_wrapper<vax::MeshManager> _meshManager;
    std::reference_wrapper<vax::MaterialManager> _materialManager;
    std::reference_wrapper<vax::vk::CommandManager> _commandManager;
    std::reference_wrapper<vax::vk::QueueManager> _queueManager;
};
} // namespace vax::objects