#include "drawableScene.h"
#include "gridWorldDescriptor.h"
#include "modelLoader.h"
#include "primitivesBuilder.h"
#include "swapchain.h"
#include "textureLoader.h"

using namespace vax;

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
}

void vax::DrawableScene::resize() {
    auto swapchainExtent = _vkEngine.get().swapchain->swapchainExtent;
    _mainCamera.setViewPortSize(vax::math::SizeUI(swapchainExtent));
}

void vax::DrawableScene::loadGridWorld(
    const vax::rl::gw::env::GridWorldDrawableDescriptor& gridWorldDrawableDescriptor, VkQueue submitQueue
) {
    _resourceManager.setup();
    _drawableModels.reserve(gridWorldDrawableDescriptor.drawableDescriptors.size());
    for (const auto& drawableDescriptor : gridWorldDrawableDescriptor.drawableDescriptors) {
        auto model = _modelLoader.loadModel(drawableDescriptor.path, submitQueue);
        model->transformHandle.setTransform(drawableDescriptor.initialTransform);
        if (!model.has_value()) {
            _logger.error("Failed to load model: {}", drawableDescriptor.path);
            continue;
        }
        _drawableModels.push_back(std::move(model.value()));
    }
    _load(submitQueue);
}

void vax::DrawableScene::_load(VkQueue submitQueue) {
    _loadEnvironmentMap(submitQueue);
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vax::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vax::MAX_FRAMES_IN_FLIGHT; i++) {
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
    _background = _primitivesBuilder.createBackground();
    if (!_background) {
        _logger.error("Failed to create background!");
        return;
    }
    _gizmo = _modelLoader.loadModel(RES_PATH("assets/models/gizmo.glb"), submitQueue);
    _gizmo->setSettings({.precomputedMVP = true});

    auto commandBuffer = _vkEngine.get().commandManager->createSingleTimeCommandBuffer();
    commandBuffer.begin();
    _gizmo->loadMesh(commandBuffer);
    _background->loadMesh(commandBuffer);
    for (auto& drawableModel : _drawableModels) {
        drawableModel.loadMesh(commandBuffer);
    }
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
    auto cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
    _mainCamera.setPosition(cameraPos);
    _gizmoCamera.setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    _gizmoCamera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    _gizmoCamera.setViewPortSize(math::SizeUI(256, 256));
    _gizmoCamera.setProjection(objects::Camera::Projection::orthographic);
    _gizmoCamera.setViewSize(1.5f);
}

bool vax::DrawableScene::writeGlobalDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter) {
    auto globalSampler = _resourceManager.textureManager().getGlobalSampler(GlobalSampler::PBRSampler);
    if (!globalSampler.has_value()) {
        return false;
    }
    descriptorSetWriter.writeBuffer(
        _resourceManager.materialManager().materialBuffer(),
        GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    descriptorSetWriter.writeBuffer(
        _environmentMap->environmentMapBuffer(),
        GlobalBindingIndices::GLOBAL_ENVIRONMENT_MAP_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    descriptorSetWriter.writeSampler(*globalSampler->second, GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, 0);
    _resourceManager.textureManager().updateDescriptorWriterWithAllTextures(
        descriptorSetWriter, GlobalBindingIndices::GLOBAL_TEXTURE_INDEX, false
    );
    return true;
}

bool vax::DrawableScene::writeFrameDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter) {
    descriptorSetWriter.writeBuffer(
        *_sceneUniformBuffers[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX,
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );
    return true;
}

void vax::DrawableScene::draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    for (auto& drawableModel : _drawableModels) {
        drawableModel.draw(&_vkEngine.get(), commandBuffer, pipeline.vkPipelineLayout, _sceneUpdateContext.deltaTime);
    }
}

void vax::DrawableScene::drawBackground(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    if (!_background)
        return;
    _background->draw(&_vkEngine.get(), commandBuffer, pipeline.vkPipelineLayout, _sceneUpdateContext.deltaTime);
}

void vax::DrawableScene::drawGizmo(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    if (!_gizmo)
        return;
    auto viewMatrix = _gizmoCamera.viewMatrix();
    auto projectionMatrix = _gizmoCamera.projectionMatrix();
    auto viewProjectionMatrix = projectionMatrix * viewMatrix;
    _gizmo->transformHandle.setModelMatrix(viewProjectionMatrix);
    _gizmo->draw(&_vkEngine.get(), commandBuffer, pipeline.vkPipelineLayout, _sceneUpdateContext.deltaTime);
}

void vax::DrawableScene::onMouseMove(const vax::input::MouseMoveValue& value) {
    _mainCamera.rotateBy(value.delta);
    _gizmoCamera.rotateBy(value.delta);
}

void vax::DrawableScene::_loadEnvironmentMap(VkQueue submitQueue) {
    _environmentMap->load(
        {.textures =
             {{scene::EnvironmentMap::TextureType::BRDFLUT, RES_PATH("brdf/brdfLUT.ktx")},
              {scene::EnvironmentMap::TextureType::EnvMapIrradiance, RES_PATH("brdf/irradiance.ktx")},
              {scene::EnvironmentMap::TextureType::EnvMap, RES_PATH("brdf/prefilter.ktx")}}},
        submitQueue
    );
}