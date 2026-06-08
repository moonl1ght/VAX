#pragma once

#include "drawableModel.h"
#include "luna.h"
#include "resourceManager.h"
#include "textureLoader.h"

namespace vax::objects {
class ModelLoader final {
  public:
    explicit ModelLoader(vax::ResourceManager& resourceManager, vax::textures::TextureLoader& textureLoader)
        : _resourceManager(resourceManager)
        , _textureLoader(textureLoader) {};

    ~ModelLoader() {};

    ModelLoader(const ModelLoader& other) = delete;
    ModelLoader& operator=(const ModelLoader& other) = delete;
    ModelLoader(ModelLoader&& other) noexcept = delete;
    ModelLoader& operator=(ModelLoader&& other) noexcept = delete;

    std::optional<DrawableModel> loadModel(const std::string& path, VkQueue submitQueue);
    std::optional<DrawableModel> loadURDFModel(const std::string& path, VkQueue submitQueue);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("ModelLoader");

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::textures::TextureLoader> _textureLoader;
};
} // namespace vax::objects