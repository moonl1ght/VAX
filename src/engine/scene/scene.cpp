#include "scene.h"
#include "textureLoader.h"
#include "modelLoader.h"
#include "primitives2d.h"
#include "swapchain.h"

void vax::Scene::update(float deltaTime) {
    _ubo.model = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.proj = glm::perspective(
        glm::radians(45.0f), 
        vkEngine->swapchain->swapchainExtent.width / (float) vkEngine->swapchain->swapchainExtent.height, 0.1f, 10.0f
    );
    _ubo.proj[1][1] *= -1;
}

void vax::Scene::load() {
    texture = TextureLoader(vkEngine).loadTexture(RES_PATH("assets/models/room/viking_room.png"));
    _ubo.model = glm::mat4(1.0f);
    _ubo.view = glm::mat4(1.0f);
    _ubo.proj = glm::mat4(1.0f);
    _drawableModels.emplace_back(_modelLoader.loadModel(RES_PATH("assets/models/room/viking_room.obj")).value());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // // _drawableModels.emplace_back(Primitives2D::createPlane());
    // _drawableModels[1]->transform.position = glm::vec3(0.0f, 0.0f, -0.5f);
    for (auto& model : _drawableModels) {
        model->mesh->loadBuffers(*vkEngine->queueManager, *vkEngine->commandManager);
    }
}