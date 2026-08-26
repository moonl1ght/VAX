#include "drawableScene.h"
#include "camera.h"
#include "gridWorldDescriptor.h"
#include "swapchain.h"

using namespace vax;
using namespace vax::vk;
using namespace vax::engine;
using namespace vax::rl;

void DrawableScene::prepareForDraw(engine::RenderCallContext renderCallContext) {
    _renderCallContext = renderCallContext;
    if (auto mappedMemory = _sceneUniformBuffers[renderCallContext.currentFrame]->mappedMemory()) {
        uint8_t* mappedPtr = static_cast<uint8_t*>(mappedMemory.value());
        auto passUboStride = _vkEngine.get().device->minUniformBufferOffsetAlignment<UniformBufferObject>();
        memcpy(mappedPtr + (0 * passUboStride), &_ubo, sizeof(_ubo));
        memcpy(mappedPtr + (1 * passUboStride), &_sunLightUbo, sizeof(_sunLightUbo));
    } else {
        _logger.error("Failed to get mapped memory!");
    }

    if (auto mappedMemory = _roverCameraUniformBuffers[renderCallContext.currentFrame]->mappedMemory()) {
        memcpy(mappedMemory.value(), &_roverCameraUbo, sizeof(_roverCameraUbo));
    } else {
        _logger.error("Failed to get mapped memory!");
    }

    auto lightMappedMemory = _lightsUniformBuffer[renderCallContext.currentFrame]->mappedMemory();
    if (lightMappedMemory.has_value()) {
        auto lightData = static_cast<LightUBO*>(lightMappedMemory.value());
        lightData->lightCount = 1;
        auto sunLightUbo = _sunLight.lightUBOIndex();
        lightData->lights[sunLightUbo].lightSpaceMatrix =
            _sunLight.camera().projectionMatrix() * _sunLight.camera().viewMatrix();
        lightData->lights[sunLightUbo].position = glm::vec4(_sunLight.camera().position(), 1.0f);
        lightData->lights[sunLightUbo].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        lightData->lights[sunLightUbo].shadowMapIndex = 0;
        lightData->lights[sunLightUbo].radius = 10.0f;
        lightData->lights[sunLightUbo].shadowMapSamplerIndex = 0;
    } else {
        _logger.error("Failed to get mapped memory!");
    }
}

void DrawableScene::update(engine::SceneUpdateContext sceneUpdateContext) {
    _sceneUpdateContext = sceneUpdateContext;
    _ubo = _mainCamera.getUniformBufferObject();
    _ubo.environmentMapIndex = 0;
    _sceneGraph->update(sceneUpdateContext.frameTime);

    auto& roverCamera = _sceneGraph->roverCamera();
    _roverCameraUbo = roverCamera.getUniformBufferObject();
    _roverCameraUbo.environmentMapIndex = 0;
    _sunLightUbo = _sunLight.camera().getUniformBufferObject();
    _sunLightUbo.environmentMapIndex = 0;
}

void vax::engine::DrawableScene::resize() {
    auto swapchainExtent = _vkEngine.get().getWindowController().getWindow(0)->getSwapchain()->swapchainExtent;
    _mainCamera.setViewPortSize(vax::math::SizeUI(swapchainExtent));
    _sunLight.camera().setViewPortSize(vax::math::SizeUI(swapchainExtent));
}

void vax::engine::DrawableScene::loadScene(const GridWorldDrawableDescriptor& descriptor, VkQueue submitQueue) {
    _resourceManager.setup(_modelsController.maxDrawableInstances());
    _sceneGraph = std::make_unique<GwSceneGraph>();
    _loadEnvironmentMap(submitQueue);
    uint32_t lightCount = 1;
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vax::vk::MAX_FRAMES_IN_FLIGHT);
    _roverCameraUniformBuffers.reserve(vax::vk::MAX_FRAMES_IN_FLIGHT);
    _lightsUniformBuffer.reserve(vax::vk::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto& bufferManager = _resourceManager.bufferManager();
        auto passUboStride = _vkEngine.get().device->minUniformBufferOffsetAlignment<UniformBufferObject>();
        auto allocation = bufferManager
                              .allocateBuffer(
                                  "frame_uniform_buffer",
                                  passUboStride * (lightCount + 1),
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                              )
                              .value();
        allocation.second->map();
        _sceneUniformBuffers.push_back(allocation.second);
        auto roverCameraAllocation = bufferManager
                                         .allocateBuffer(
                                             "rover_camera_uniform_buffer",
                                             bufferSize,
                                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                         )
                                         .value();
        roverCameraAllocation.second->map();
        _roverCameraUniformBuffers.push_back(roverCameraAllocation.second);
        auto lightAllocation = bufferManager
                                   .allocateBuffer(
                                       "light_uniform_buffer",
                                       sizeof(LightUBO),
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                   )
                                   .value();
        lightAllocation.second->map();
        _lightsUniformBuffer.push_back(lightAllocation.second);
    }
    std::vector<vax::engine::ModelDescriptor> modelDescriptors = {
        {
        .path = "",
        .id = "background",
        .modelType = vax::engine::ModelDescriptor::ModelType::PRIMITIVE_PLANE,
        },
        {
        .path = RES_PATH("assets/models/gizmo.glb"),
        .id = "gizmo",
        .modelType = vax::engine::ModelDescriptor::ModelType::MODEL,
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
        drawableModel->setSettings({.precomputedMVP = true});
    }
    _background = std::move(_modelsController.createSceneNodeById("background"));

    auto commandBuffer = _vkEngine.get().commandManager->createSingleTimeCommandBuffer();

    commandBuffer.begin();
    _modelLoader.loadStaged(commandBuffer);
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
    _modelLoader.cleanupStaged();

    auto sunCamera = Camera();
    sunCamera.setPosition(glm::vec3(1.0f, 5.0f, 3.0f));
    auto swapchainExtent = _vkEngine.get().getWindowController().getWindow(0)->getSwapchain()->swapchainExtent;
    sunCamera.setViewPortSize(vax::math::SizeUI(swapchainExtent));
    sunCamera.setProjection(Camera::Projection::orthographic);
    sunCamera.setViewSize(10.0f);
    _sunLight = Light(sunCamera);
    _sunLight.setLightUBOIndex(0);

    auto cameraPos = glm::vec3(1.0f, 5.0f, -3.0f);
    _mainCamera.setPosition(cameraPos);
    _gizmoCamera.setPosition(cameraPos);
    _gizmoCamera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    _gizmoCamera.setViewPortSize(math::SizeUI(256, 256));
    _gizmoCamera.setProjection(engine::Camera::Projection::orthographic);
    _gizmoCamera.setViewSize(1.5f);
}

bool vax::engine::DrawableScene::writeGlobalDescriptorSet(vax::vk::DescriptorSetHandler& descriptorHandler) {
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

bool vax::engine::DrawableScene::writeFrameDescriptorSet(
    vax::vk::DescriptorSetHandler& descriptorHandler, vax::vk::DescriptorSetHandler& roverCameraDescriptorHandler
) {
    descriptorHandler.writeBuffer(
        *_sceneUniformBuffers[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    );
    descriptorHandler.writeBuffer(
        *_lightsUniformBuffer[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_LIGHT_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );
    descriptorHandler.writeBuffer(
        _resourceManager.ssboManager().instanceBuffer(_renderCallContext.currentFrame),
        FrameBindingIndices::FRAME_INSTANCE_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );

    roverCameraDescriptorHandler.writeBuffer(
        *_roverCameraUniformBuffers[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    );
    roverCameraDescriptorHandler.writeBuffer(
        *_lightsUniformBuffer[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_LIGHT_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );
    roverCameraDescriptorHandler.writeBuffer(
        _resourceManager.ssboManager().instanceBuffer(_renderCallContext.currentFrame),
        FrameBindingIndices::FRAME_INSTANCE_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    return true;
}

void vax::engine::DrawableScene::draw(const DrawContext& drawContext) { _sceneGraph->draw(drawContext); }

void vax::engine::DrawableScene::drawBackground(const DrawContext& drawContext) {
    if (!_background)
        return;
    _background->draw(drawContext);
}

void vax::engine::DrawableScene::drawGizmo(const DrawContext& drawContext) {
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

void vax::engine::DrawableScene::onMouseMove(const vax::MouseMoveValue& value) {
    _mainCamera.rotateBy(value.delta);
    _gizmoCamera.rotateBy(value.delta);
}

void vax::engine::DrawableScene::onMouseWheel(float delta) { _mainCamera.zoomBy(0.1f * delta); }

void vax::engine::DrawableScene::onKeyEvent(const vax::KeyEvent& keyEvent) {}

void vax::engine::DrawableScene::_loadEnvironmentMap(VkQueue submitQueue) {
    _environmentMap->load(
        {
        .textures =
            {
            {engine::EnvironmentMap::TextureType::BRDFLUT, RES_PATH("brdf/brdfLUT.ktx")},
            {engine::EnvironmentMap::TextureType::EnvMapIrradiance, RES_PATH("brdf/irradiance.ktx")},
            {engine::EnvironmentMap::TextureType::EnvMap, RES_PATH("brdf/prefilter.ktx")},
            },
        },
        submitQueue
    );
}