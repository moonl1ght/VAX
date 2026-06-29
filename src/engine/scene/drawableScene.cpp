#include "drawableScene.h"
#include "gridWorldDescriptor.h"
#include "swapchain.h"

using namespace vax;
using namespace vax::renderer;
using namespace vax::rl::gw::env;

void DrawableScene::prepareForDraw(renderer::RenderCallContext renderCallContext) {
    _renderCallContext = renderCallContext;
    if (auto mappedMemory = _sceneUniformBuffers[renderCallContext.currentFrame]->mappedMemory()) {
        memcpy(mappedMemory.value(), &_ubo, sizeof(_ubo));
    } else {
        _logger.error("Failed to get mapped memory!");
    }
}

void DrawableScene::update(SceneUpdateContext sceneUpdateContext) {
    _sceneUpdateContext = sceneUpdateContext;
    _ubo = _mainCamera.getUniformBufferObject();
    _ubo.environmentMapIndex = 0;
    _sceneGraph->update(sceneUpdateContext.deltaTime);
}

void vax::DrawableScene::resize() {
    auto swapchainExtent = _vkEngine.get().swapchain->swapchainExtent;
    _mainCamera.setViewPortSize(vax::math::SizeUI(swapchainExtent));
}

void vax::DrawableScene::loadScene(const GridWorldDrawableDescriptor& descriptor, VkQueue submitQueue) {
    _resourceManager.setup(_modelsController.maxDrawableInstances());
    _sceneGraph = std::make_unique<vax::rl::gw::GwSceneGraph>();
    _loadEnvironmentMap(submitQueue);
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vax::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vax::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto& bufferManager = _resourceManager.bufferManager();
        auto allocation = bufferManager
                              .allocateBuffer(
                                  "frame_uniform_buffer",
                                  bufferSize,
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                              )
                              .value();
        allocation.second->map();
        _sceneUniformBuffers.push_back(allocation.second);
    }
    std::vector<vax::objects::ModelDescriptor> modelDescriptors = {
        {
        .path = "",
        .id = "background",
        .modelType = vax::objects::ModelDescriptor::ModelType::PRIMITIVE_PLANE,
        },
        {
        .path = RES_PATH("assets/models/gizmo.glb"),
        .id = "gizmo",
        .modelType = vax::objects::ModelDescriptor::ModelType::MODEL,
        }
    };
    for (const auto& drawableDescriptor : descriptor.drawableDescriptors) {
        modelDescriptors.push_back(drawableDescriptor);
    }
    modelDescriptors.push_back(descriptor.agentDrawableDescriptor);
    auto commandBuffer1 = _vkEngine.get().commandManager->createSingleTimeCommandBuffer();
    _modelsController.preload(modelDescriptors, commandBuffer1, submitQueue);
    _sceneGraph->load(_modelsController, descriptor);
    _gizmo = std::move(_modelsController.createSceneNodeById("gizmo"));
    for (auto& drawableModel : _gizmo->drawableModels()) {
        drawableModel->setSettings({.precomputedMVP = true, .instanceDrawing = true});
    }
    _background = std::move(_modelsController.createSceneNodeById("background"));

    auto commandBuffer = _vkEngine.get().commandManager->createSingleTimeCommandBuffer();

    commandBuffer.begin();
    _modelLoader.loadStaged(commandBuffer);
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
    _modelLoader.cleanupStaged();

    auto cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
    _mainCamera.setPosition(cameraPos);
    _gizmoCamera.setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    _gizmoCamera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    _gizmoCamera.setViewPortSize(math::SizeUI(256, 256));
    _gizmoCamera.setProjection(objects::Camera::Projection::orthographic);
    _gizmoCamera.setViewSize(1.5f);
}

bool vax::DrawableScene::writeGlobalDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler) {
    auto globalSampler = _resourceManager.textureManager().getGlobalSampler(GlobalSampler::PBRSampler);
    auto globalCubeMapSampler = _resourceManager.textureManager().getGlobalSampler(GlobalSampler::CubeMapSampler);
    if (!globalSampler.has_value() || !globalCubeMapSampler.has_value()) {
        return false;
    }
    descriptorHandler.writeBuffer(
        _resourceManager.materialManager().materialBuffer(),
        GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    descriptorHandler.writeBuffer(
        _environmentMap->environmentMapBuffer(),
        GlobalBindingIndices::GLOBAL_ENVIRONMENT_MAP_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    descriptorHandler.writeSampler(*globalSampler->second, GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, 0);
    descriptorHandler.writeSampler(*globalCubeMapSampler->second, GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, 1);
    _resourceManager.textureManager().updateDescriptorHandlerWithAllTextures(
        descriptorHandler, GlobalBindingIndices::GLOBAL_TEXTURE_INDEX
    );
    return true;
}

bool vax::DrawableScene::writeFrameDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler) {
    descriptorHandler.writeBuffer(
        *_sceneUniformBuffers[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );
    descriptorHandler.writeBuffer(
        _resourceManager.ssboManager().instanceBuffer(_renderCallContext.currentFrame),
        FrameBindingIndices::FRAME_INSTANCE_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    return true;
}

void vax::DrawableScene::draw(const DrawContext& drawContext) { _sceneGraph->draw(drawContext); }

void vax::DrawableScene::drawBackground(const DrawContext& drawContext) {
    if (!_background)
        return;
    _background->draw(drawContext);
}

void vax::DrawableScene::drawGizmo(const DrawContext& drawContext) {
    if (!_gizmo)
        return;
    auto viewMatrix = _gizmoCamera.viewMatrix();
    auto projectionMatrix = _gizmoCamera.projectionMatrix();
    auto viewProjectionMatrix = projectionMatrix * viewMatrix;
    _gizmo->updateTransform([&](vax::math::TransformHandle& transformHandle) {
        transformHandle.setCachedTransformMatrix(viewProjectionMatrix);
    });
    _gizmo->draw(drawContext);
}

void vax::DrawableScene::onMouseMove(const vax::input::MouseMoveValue& value) {
    _mainCamera.rotateBy(value.delta);
    _gizmoCamera.rotateBy(value.delta);
}

void vax::DrawableScene::onMouseWheel(float delta) { _mainCamera.zoomBy(0.1f * delta); }

void vax::DrawableScene::onKeyEvent(const vax::input::KeyEvent& keyEvent) {}

void vax::DrawableScene::_loadEnvironmentMap(VkQueue submitQueue) {
    _environmentMap->load(
        {
        .textures =
            {
            {scene::EnvironmentMap::TextureType::BRDFLUT, RES_PATH("brdf/brdfLUT.ktx")},
            {scene::EnvironmentMap::TextureType::EnvMapIrradiance, RES_PATH("brdf/irradiance.ktx")},
            {scene::EnvironmentMap::TextureType::EnvMap, RES_PATH("brdf/prefilter.ktx")},
            },
        },
        submitQueue
    );
}