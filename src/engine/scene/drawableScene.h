#pragma once

#include "camera.h"
#include "descriptorSetHandler.h"
#include "drawContext.h"
#include "drawableModel.h"
#include "environmentMap.h"
#include "frameTime.h"
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
#include "light.h"

namespace vax::rl {
struct GridWorldDrawableDescriptor;
} // namespace vax::rl

namespace vax::engine {
struct SceneUpdateContext {
    FrameTime frameTime;
};

class DrawableScene final : public vax::InputController::Observer {
  public:
    DrawableScene(vax::vk::Engine& vkEngine)
        : _vkEngine(vkEngine)
        , _resourceManager(vax::vk::ResourceManager(*vkEngine.device, vkEngine.allocator))
        , _textureLoader(
              vax::vk::TextureLoader(*vkEngine.device, _resourceManager.textureManager(), *vkEngine.commandManager)
          )
        , _modelLoader(vax::engine::ModelLoader(_resourceManager, _textureLoader))
        , _primitivesBuilder(
              vax::engine::PrimitivesBuilder(
                  _resourceManager.meshManager(),
                  _resourceManager.ssboManager(),
                  _resourceManager.materialManager(),
                  *_vkEngine.get().commandManager,
                  *_vkEngine.get().queueManager
              )
          )
        , _modelsController(_resourceManager, _modelLoader, _primitivesBuilder) {
        _environmentMap = std::make_optional<vax::engine::EnvironmentMap>(_textureLoader, *vkEngine.device);
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

    const vax::engine::Camera& gizmoCamera() const { return _gizmoCamera; }

    void loadScene(const vax::rl::GridWorldDrawableDescriptor& descriptor, VkQueue submitQueue);

    void resize();

    void prepareForDraw(vax::engine::RenderCallContext renderCallContext);

    void update(vax::engine::SceneUpdateContext sceneUpdateContext);

    bool writeGlobalDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler);

    bool writeFrameDescriptorSet(
        vax::vk::DescriptorSetHandler& descriptorHandler, vax::vk::DescriptorSetHandler& roverCameraDescriptorHandler
    );

    void draw(const vax::engine::DrawContext& drawContext);

    void drawBackground(const vax::engine::DrawContext& drawContext);

    void drawGizmo(const vax::engine::DrawContext& drawContext);

    void onMouseMove(const vax::MouseMoveValue& value);

    void onMouseWheel(float delta);

    void onKeyEvent(const vax::KeyEvent& keyEvent);

    vax::rl::GwSceneGraph* sceneGraph() const { return _sceneGraph.get(); }

    bool shouldDrawSecondaryWindow() const { return _shouldDrawSecondaryWindow; }

    void setShouldDrawSecondaryWindow(bool shouldDrawSecondaryWindow) {
        _shouldDrawSecondaryWindow = shouldDrawSecondaryWindow;
    }

  private:
    vax::Logger _logger = vax::Logger("DrawableScene");
    std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
    std::vector<vax::vk::Buffer*> _roverCameraUniformBuffers;
    std::vector<vax::vk::Buffer*> _lightsUniformBuffer;
    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    vax::engine::ModelsController _modelsController;
    vax::vk::ResourceManager _resourceManager;
    vax::vk::TextureLoader _textureLoader;
    vax::engine::ModelLoader _modelLoader;
    vax::engine::PrimitivesBuilder _primitivesBuilder;
    vax::engine::Camera _mainCamera;
    vax::engine::Camera _gizmoCamera;
    vax::engine::Light _sunLight;
    UniformBufferObject _ubo;
    UniformBufferObject _sunLightUbo;
    UniformBufferObject _roverCameraUbo;
    std::unique_ptr<vax::rl::GwSceneGraph> _sceneGraph;
    std::optional<vax::engine::SceneNode> _background;
    std::optional<vax::engine::SceneNode> _gizmo;
    std::optional<vax::engine::EnvironmentMap> _environmentMap;

    vax::engine::RenderCallContext _renderCallContext;
    vax::engine::SceneUpdateContext _sceneUpdateContext;

    bool _shouldDrawSecondaryWindow = false;

    void _loadEnvironmentMap(VkQueue submitQueue);
    void _drawSceneNode(vax::engine::SceneNode& node, VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);
};
} // namespace vax::engine