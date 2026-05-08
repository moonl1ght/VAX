#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "meshManager.h"
#include "materialManager.h"
#include "colorPalette.h"

namespace vax::vk {
    class CommandManager;
    class QueueManager;
}

namespace vax::objects {
    class PrimitivesBuilder {
    public:
        explicit PrimitivesBuilder(
            vax::MeshManager& meshManager,
            vax::MaterialManager& materialManager,
            vax::vk::CommandManager& commandManager,
            vax::vk::QueueManager& queueManager
        )
            : _meshManager(meshManager)
            , _materialManager(materialManager)
            , _commandManager(commandManager)
            , _queueManager(queueManager) {
        };

        ~PrimitivesBuilder() {};

        PrimitivesBuilder(const PrimitivesBuilder& other) = delete;
        PrimitivesBuilder(PrimitivesBuilder&& other) noexcept = delete;
        PrimitivesBuilder& operator=(const PrimitivesBuilder& other) = delete;
        PrimitivesBuilder& operator=(PrimitivesBuilder&& other) noexcept = delete;

        std::optional<vax::objects::DrawableModel> createCube(float size, vax::Color color);

    private:
        std::reference_wrapper<vax::MeshManager> _meshManager;
        std::reference_wrapper<vax::MaterialManager> _materialManager;
        std::reference_wrapper<vax::vk::CommandManager> _commandManager;
        std::reference_wrapper<vax::vk::QueueManager> _queueManager;
    };
}