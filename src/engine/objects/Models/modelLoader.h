#pragma once

#include "drawableModel.h"
#include "luna.h"
#include "modelDescriptor.h"
#include "resourceManager.h"
#include "sceneNode.h"
#include "textureLoader.h"

namespace vax::objects {
class ModelsController;
} // namespace vax::objects

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
    loadSceneModel(vax::objects::ModelsController& modelsController, const vax::objects::ModelDescriptor& descriptor);

    std::optional<DrawableModel>
    loadModel(const std::string& path, uint32_t instancesCount = 1);

    void loadStaged(vax::vk::CommandBuffer& commandBuffer);
    void cleanupStaged();

  private:
    vax::Logger _logger = vax::Logger("ModelLoader");

    std::reference_wrapper<vax::ResourceManager> _resourceManager;
    std::reference_wrapper<vax::textures::TextureLoader> _textureLoader;

    std::optional<SceneNode>
    _loadURDFSceneModel(vax::objects::ModelsController& modelsController, vax::objects::ModelDescriptor descriptor);
    std::optional<SceneNode>
    _loadGLBSceneModel(vax::objects::ModelsController& modelsController, vax::objects::ModelDescriptor descriptor);
};
} // namespace vax::objects