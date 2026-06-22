#pragma once

#include "drawableModel.h"
#include "loaderDescriptor.h"
#include "luna.h"
#include "resourceManager.h"
#include "sceneNode.h"
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

    std::optional<SceneNode>
    loadSceneModel(const vax::objects::LoaderDescriptor& descriptor, VkQueue submitQueue = nullptr);

    std::optional<DrawableModel> loadModel(const std::string& path, VkQueue submitQueue = nullptr);

    void loadStaged(vax::vk::CommandBuffer& commandBuffer);
    void cleanupStaged();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("ModelLoader");

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::textures::TextureLoader> _textureLoader;

    std::optional<SceneNode> _loadURDFSceneModel(vax::objects::LoaderDescriptor descriptor, VkQueue submitQueue);
    std::optional<SceneNode> _loadGLBSceneModel(vax::objects::LoaderDescriptor descriptor, VkQueue submitQueue);
};
} // namespace vax::objects