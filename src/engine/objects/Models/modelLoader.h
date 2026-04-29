#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "resourceManager.h"

namespace vax::objects {
    class ModelLoader final {
    public:
        explicit ModelLoader(
            vax::ResourceManager& resourceManager
        ) : _resourceManager(resourceManager) {};
        ~ModelLoader() {};

        ModelLoader(const ModelLoader& other) = delete;
        ModelLoader& operator=(const ModelLoader& other) = delete;
        ModelLoader(ModelLoader&& other) noexcept = delete;
        ModelLoader& operator=(ModelLoader&& other) noexcept = delete;

        std::optional<DrawableModel> loadModel(const std::string& path);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("ModelLoader");

        std::reference_wrapper<vax::ResourceManager> _resourceManager;
    };
}