#include "Scene.hpp"
#include "TextureLoader.hpp"
#include "Primitives2D.hpp"

void Scene::update(float deltaTime) {
    _ubo.model = glm::rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _ubo.proj = glm::perspective(
        glm::radians(45.0f), _vkStack->swapChainExtent.width / (float)_vkStack->swapChainExtent.height, 0.1f, 10.0f
    );
    _ubo.proj[1][1] *= -1;
}

void Scene::load() {
    texture = TextureLoader::loadTexture(_vkStack, RES_PATH("images/texture.jpg"));
    _ubo.model = glm::mat4(1.0f);
    _ubo.view = glm::mat4(1.0f);
    _ubo.proj = glm::mat4(1.0f);
    _drawableModels.emplace_back(Primitives2D::createPlane());
    for (auto model : _drawableModels) {
        model->mesh->loadBuffers(_vkStack);
    }
}