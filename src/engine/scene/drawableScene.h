#pragma once

#include "camera.h"
#include "descriptorSetHandler.h"
#include "drawContext.h"
#include "drawableModel.h"
#include "environmentMap.h"
#include "gwSceneGraph.h"
#include "inputController.h"
#include "luna.h"
#include "modelLoader.h"
#include "modelsController.h"
#include "pipeline.h"
#include "primitivesBuilder.h"
#include "renderContext.h"
#include "resourceManager.h"
#include "sceneNode.h"
#include "shaderUniforms.h"
#include "textureLoader.h"
#include "vkEngine.h"

namespace vax::rl::gw::env {
struct GridWorldDrawableDescriptor;
} // namespace vax::rl::gw::env

namespace vax {
struct SceneUpdateContext {
    float deltaTime;
};

class DrawableScene final : public vax::input::InputController::Observer {
  public:
    DrawableScene(vax::vk::Engine& vkEngine)
        : _vkEngine(vkEngine)
        , _resourceManager(vax::ResourceManager(*vkEngine.device, vkEngine.allocator))
        , _textureLoader(
              vax::textures::TextureLoader(
                  *vkEngine.device, _resourceManager.textureManager(), *vkEngine.commandManager
              )
          )
        , _modelLoader(vax::objects::ModelLoader(_resourceManager, _textureLoader))
        , _primitivesBuilder(
              vax::objects::PrimitivesBuilder(
                  _resourceManager.meshManager(),
                  _resourceManager.ssboManager(),
                  _resourceManager.materialManager(),
                  *_vkEngine.get().commandManager,
                  *_vkEngine.get().queueManager
              )
          )
        , _modelsController(_resourceManager, _modelLoader, _primitivesBuilder) {
        _environmentMap = std::make_optional<vax::scene::EnvironmentMap>(_textureLoader, *vkEngine.device);
    };

    ~DrawableScene() {
        _resourceManager.cleanup();
        if (_inputController) {
            _inputController->removeObserver(this);
        }
    };

    DrawableScene(const DrawableScene& other) = delete;
    DrawableScene& operator=(const DrawableScene& other) = delete;
    DrawableScene(DrawableScene&& other) noexcept = delete;
    DrawableScene& operator=(DrawableScene&& other) noexcept = delete;

    const vax::objects::Camera& gizmoCamera() const { return _gizmoCamera; }

    void loadScene(const vax::rl::gw::env::GridWorldDrawableDescriptor& descriptor, VkQueue submitQueue);

    void resize();

    void prepareForDraw(vax::renderer::RenderCallContext renderCallContext);

    void update(vax::SceneUpdateContext sceneUpdateContext);

    bool writeGlobalDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler);

    bool writeFrameDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler);

    void draw(const vax::renderer::DrawContext& drawContext);

    void drawBackground(const vax::renderer::DrawContext& drawContext);

    void drawGizmo(const vax::renderer::DrawContext& drawContext);

    void onMouseMove(const vax::input::MouseMoveValue& value);

    void onMouseWheel(float delta);

    void onKeyEvent(const vax::input::KeyEvent& keyEvent);

    vax::rl::gw::GwSceneGraph* sceneGraph() const { return _sceneGraph.get(); }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("DrawableScene");
    std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    vax::objects::ModelsController _modelsController;
    vax::ResourceManager _resourceManager;
    vax::textures::TextureLoader _textureLoader;
    vax::objects::ModelLoader _modelLoader;
    vax::objects::PrimitivesBuilder _primitivesBuilder;
    vax::objects::Camera _mainCamera;
    vax::objects::Camera _gizmoCamera;
    UniformBufferObject _ubo;
    std::unique_ptr<vax::rl::gw::GwSceneGraph> _sceneGraph;
    std::optional<vax::objects::SceneNode> _background;
    std::optional<vax::objects::SceneNode> _gizmo;
    std::optional<vax::scene::EnvironmentMap> _environmentMap;

    vax::renderer::RenderCallContext _renderCallContext;
    vax::SceneUpdateContext _sceneUpdateContext;

    void _loadEnvironmentMap(VkQueue submitQueue);
    void
    _drawSceneNode(vax::objects::SceneNode& node, VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);
};
} // namespace vax