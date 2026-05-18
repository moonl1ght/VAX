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
    _ubo = _mainCamera.getUniformBufferObject();
}

void vax::DrawableScene::resize() {
    auto swapchainExtent = _vkEngine.get().swapchain->swapchainExtent;
    _mainCamera.setViewPortSize(vax::math::SizeUI(swapchainExtent));
}

void vax::DrawableScene::load(VkQueue submitQueue) {
    _resourceManager.setup();
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vax::MAX_FRAMES_IN_FLIGHT);
    // _sceneUniformBuffersMapped.resize(vax::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vax::MAX_FRAMES_IN_FLIGHT; i++) {
        auto& bufferManager = _resourceManager.bufferManager();
        auto allocation = bufferManager.allocateBuffer(
            "frame_uniform_buffer",
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
    auto gizmo = _modelLoader.loadModel(RES_PATH("assets/models/gizmo.glb"), submitQueue);
    auto helmet = _modelLoader.loadModel(RES_PATH("assets/models/helmet.glb"), submitQueue);
    // auto cube = _primitivesBuilder.createCube(1.0f, vax::ColorPalette::Gray);
    auto commandBuffer = _vkEngine.get().commandManager->createSingleTimeCommandBuffer();
    commandBuffer.begin();
    gizmo->loadMesh(commandBuffer);
    helmet->loadMesh(commandBuffer);
    // cube->loadMesh(commandBuffer);
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
    // _vkEngine.get().commandManager->endSingleTimeCommands(commandBuffer);
    // model->loadMesh(*_vkEngine.get().queueManager, *_vkEngine.get().commandManager);
    // texture = TextureLoader(vkEngine).loadTexture(RES_PATH("assets/models/room/viking_room.png"));
    auto cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
    _mainCamera.setPosition(cameraPos);
    // _ubo.cameraPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // _ubo.view = glm::mat4(1.0f);
    // _ubo.proj = glm::mat4(1.0f);
    // _drawableModels.emplace_back(_modelLoader.loadModel(RES_PATH("assets/models/room/viking_room.obj")).value());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // _drawableModels[1]->transform.position = glm::vec3(0.0f, 0.0f, -0.5f);
    // auto zcube = _primitivesBuilder.createCube(0.5f, vax::ColorPalette::Blue);
    // auto ycube = _primitivesBuilder.createCube(1.0f, vax::ColorPalette::Green);
    // auto xcube = _primitivesBuilder.createCube(1.0f, vax::ColorPalette::Red);
    // xcube->transform.position = glm::vec3(2.0f, 0.0f, 0.0f);
    // ycube->transform.position = glm::vec3(0.0f, 2.0f, 0.0f);
    // zcube->transform.position = glm::vec3(0.0f, 0.0f, 2.0f);

    // _drawableModels.push_back(std::move(cube.value()));
    // _drawableModels.push_back(std::move(zcube.value()));
    _drawableModels.push_back(std::move(helmet.value()));
    _drawableModels.push_back(std::move(gizmo.value()));
    // _drawableModels.push_back(std::move(cube.value()));
    // _drawableModels.push_back(std::move(helmet.value()));
    // _drawableModels.push_back(std::move(ycube.value()));
    // _drawableModels.push_back(std::move(xcube.value()));
    // _drawableModels.push_back(std::move(cube.value()));
    // _drawableModels.push_back(std::move(zcube.value()));
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

void vax::DrawableScene::draw(VkCommandBuffer commandBuffer, const vax::vk::Pipeline& pipeline) {
    for (auto& drawableModel : _drawableModels) {
        drawableModel.draw(
            &_vkEngine.get(),
            commandBuffer, 
            pipeline.vkPipelineLayout,
            _sceneUpdateContext.deltaTime
        );
    }
}

void vax::DrawableScene::onMouseMove(const vax::input::MouseMoveValue& value) {
    _mainCamera.rotateBy(value.delta);
}