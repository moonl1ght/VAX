#include "drawableScene.h"
#include "textureLoader.h"
#include "modelLoader.h"
#include "swapchain.h"
#include "primitivesBuilder.h"

using namespace vax;

void DrawableScene::prepareForDraw(renderer::RenderCallContext renderCallContext) {
    _renderCallContext = renderCallContext;
    if (auto mappedMemory = _sceneUniformBuffers[renderCallContext.currentFrame]->mappedMemory()) {
        memcpy(mappedMemory.value(), &_ubo, sizeof(_ubo));
    }
    else {
        _logger.error("Failed to get mapped memory!");
    }
}

void DrawableScene::update(SceneUpdateContext sceneUpdateContext) {
    _sceneUpdateContext = sceneUpdateContext;
    auto cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
    _ubo.cameraPosition = glm::vec4(cameraPos, 1.0f);
    // _ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto swapchainExtent = _vkEngine.get().swapchain->swapchainExtent;
    auto swapchainWidth = swapchainExtent.width;
    auto swapchainHeight = swapchainExtent.height;
    _ubo.proj = glm::perspective(
        glm::radians(45.0f),
        swapchainWidth / (float) swapchainHeight,
        0.1f,
        10.0f
    );
    _ubo.proj[1][1] *= -1;
}

void vax::DrawableScene::load(VkQueue submitQueue) {
    _resourceManager.setup();
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vax::MAX_FRAMES_IN_FLIGHT);
    // _sceneUniformBuffersMapped.resize(vax::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vax::MAX_FRAMES_IN_FLIGHT; i++) {
        auto& bufferManager = _resourceManager.bufferManager();
        auto allocation = bufferManager.allocateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value();
        allocation.second->map();
        _sceneUniformBuffers.push_back(allocation.second);
        // vkMapMemory(
        //     _vkEngine.get().device->vkDevice,
        //     _sceneUniformBuffers[i]->vkBufferMemory(),
        //     0,
        //     bufferSize,
        //     0,
        //     &_sceneUniformBuffersMapped[i]
        // );

        // _sceneUniformBuffers[i].bind(_sceneUniformBuffersMapped[i]);
    }
    auto model = _modelLoader.loadModel(RES_PATH("assets/models/gizmo.glb"), submitQueue);
    model->loadMesh(*_vkEngine.get().queueManager, *_vkEngine.get().commandManager);
    // texture = TextureLoader(vkEngine).loadTexture(RES_PATH("assets/models/room/viking_room.png"));
    _ubo.cameraPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    _ubo.view = glm::mat4(1.0f);
    _ubo.proj = glm::mat4(1.0f);
    // _drawableModels.emplace_back(_modelLoader.loadModel(RES_PATH("assets/models/room/viking_room.obj")).value());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // _drawableModels[1]->transform.position = glm::vec3(0.0f, 0.0f, -0.5f);
    auto cube = _primitivesBuilder.createCube();
    if (!model.has_value()) {
        return;
    }
    _drawableModels.push_back(std::move(model.value()));
    // for (auto& model : _drawableModels) {
    //     model->mesh->loadBuffers(*vkEngine->queueManager, *vkEngine->commandManager);
    // }
}

bool vax::DrawableScene::writeGlobalDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter) {
    auto globalSampler = _resourceManager.textureManager().getGlobalSampler(GlobalSampler::PBRSampler);
    if (!globalSampler.has_value()) {
        return false;
    }
    descriptorSetWriter.writeBuffer(
        _resourceManager.materialManager().materialBuffer(),
        GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX, 0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    );
    descriptorSetWriter.writeSampler(
        *globalSampler->second,
        GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, 0
    );
    _resourceManager.textureManager().updateDescriptorWriterWithAllTextures(
        descriptorSetWriter,
        GlobalBindingIndices::GLOBAL_TEXTURE_INDEX,
        false
    );
    return true;
}

bool vax::DrawableScene::writeFrameDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter) {
    descriptorSetWriter.writeBuffer(
        *_sceneUniformBuffers[_renderCallContext.currentFrame],
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX, 0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );
    return true;
}

void vax::DrawableScene::draw(VkCommandBuffer commandBuffer) {
    for (auto& drawableModel : _drawableModels) {
        drawableModel.draw(
            &_vkEngine.get(),
            commandBuffer, 
            *(_vkEngine.get().pipelineManager),
            _sceneUpdateContext.deltaTime
        );
    }
}