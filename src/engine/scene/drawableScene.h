#pragma once

#include "camera.h"
#include "descriptorSetWriter.h"
#include "drawableModel.h"
#include "environmentMap.h"
#include "inputController.h"
#include "luna.h"
#include "modelLoader.h"
#include "pipeline.h"
#include "primitivesBuilder.h"
#include "renderContext.h"
#include "resourceManager.h"
#include "shaderUniforms.h"
#include "textureLoader.h"
#include "vkEngine.h"
#include "sceneNode.h"

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
                  _resourceManager.materialManager(),
                  *_vkEngine.get().commandManager,
                  *_vkEngine.get().queueManager
              )
          ) {
        _environmentMap = std::make_optional<vax::scene::EnvironmentMap>(_textureLoader, *vkEngine.device);
    };

    ~DrawableScene() {
        _drawableModels.clear();
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

    void loadGridWorld(
        const vax::rl::gw::env::GridWorldDrawableDescriptor& gridWorldDrawableDescriptor, VkQueue submitQueue
    );

    void resize();

    void prepareForDraw(vax::renderer::RenderCallContext renderCallContext);

    void update(vax::SceneUpdateContext sceneUpdateContext);

    bool writeGlobalDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter);

    bool writeFrameDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter);

    void draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);

    void drawBackground(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);

    void drawGizmo(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);

    void onMouseMove(const vax::input::MouseMoveValue& value);

    void onMouseWheel(float delta);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("DrawableScene");
    std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    vax::ResourceManager _resourceManager;
    vax::textures::TextureLoader _textureLoader;
    vax::objects::ModelLoader _modelLoader;
    vax::objects::PrimitivesBuilder _primitivesBuilder;
    vax::objects::Camera _mainCamera;
    vax::objects::Camera _gizmoCamera;
    UniformBufferObject _ubo;
    std::vector<vax::objects::DrawableModel> _drawableModels;
    std::vector<vax::objects::SceneNode> _nodes;
    std::optional<vax::objects::DrawableModel> _background;
    std::optional<vax::objects::DrawableModel> _gizmo;
    std::optional<vax::scene::EnvironmentMap> _environmentMap;

    bool _needsUpdateMaterialsSSBO = true;
    bool _needsUpdateTexturesSSBO = true;

    vax::renderer::RenderCallContext _renderCallContext;
    vax::SceneUpdateContext _sceneUpdateContext;

    void _load(VkQueue submitQueue);
    void _loadEnvironmentMap(VkQueue submitQueue);
    void _drawSceneNode(vax::objects::SceneNode& node, VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline);
};
} // namespace vax