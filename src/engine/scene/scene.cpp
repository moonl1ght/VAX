#include "scene.h"
#include "textureLoader.h"
#include "modelLoader.h"
#include "swapchain.h"
#include "primitivesBuilder.h"

using namespace vax;

void vax::Scene::update(float deltaTime) {
    // _ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

void vax::Scene::load() {
    // texture = TextureLoader(vkEngine).loadTexture(RES_PATH("assets/models/room/viking_room.png"));
    _ubo.model = glm::mat4(1.0f);
    _ubo.view = glm::mat4(1.0f);
    _ubo.proj = glm::mat4(1.0f);
    // _drawableModels.emplace_back(_modelLoader.loadModel(RES_PATH("assets/models/room/viking_room.obj")).value());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // _drawableModels[1]->transform.position = glm::vec3(0.0f, 0.0f, -0.5f);
    auto cube = _primitivesBuilder.createCube();
    if (!cube.has_value()) {
        return;
    }
    _drawableModels.push_back(std::move(cube.value()));
//     for (auto& model : _drawableModels) {
//         model->mesh->loadBuffers(*vkEngine->queueManager, *vkEngine->commandManager);
//     }
}