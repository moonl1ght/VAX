#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "texture.h"
#include "shaderUniforms.h"
#include "modelLoader.h"
#include "primitivesBuilder.h"
#include "vkEngine.h"
#include "textureLoader.h"
#include "resourceManager.h"

namespace vax {
    class DrawableScene final {
    public:

        DrawableScene(vax::vk::Engine& vkEngine)
            : _vkEngine(vkEngine)
            , _resourceManager(vax::ResourceManager(*vkEngine.device, vkEngine.allocator))
            , _textureLoader(vax::textures::TextureLoader(
                *vkEngine.device, _resourceManager.textureManager(), *vkEngine.commandManager
            ))
            , _modelLoader(vax::objects::ModelLoader(_resourceManager, _textureLoader))
            , _primitivesBuilder(vax::objects::PrimitivesBuilder(_resourceManager.meshManager())) {
        };

        ~DrawableScene() {
            _drawableModels.clear();
            _resourceManager.cleanup();
        };

        DrawableScene(const DrawableScene& other) = delete;
        DrawableScene& operator=(const DrawableScene& other) = delete;
        DrawableScene(DrawableScene&& other) noexcept = delete;
        DrawableScene& operator=(DrawableScene&& other) noexcept = delete;

        void load(VkQueue submitQueue);
        void update(float deltaTime);

        std::vector<vax::objects::DrawableModel>& getDrawableModels() {
            return _drawableModels;
        }

        const UniformBufferObject& getUBO() const {
            return _ubo;
        }

        std::vector<vax::vk::Buffer*>& getSceneUniformBuffers() {
            return _sceneUniformBuffers;
        }

        const vax::vk::Buffer& getMaterialBuffer() {
            return _resourceManager.materialManager().materialBuffer();
        }

        vax::TextureManager& textureManager() {
            return _resourceManager.textureManager();
        }

    private:
        std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
        std::reference_wrapper<vax::vk::Engine> _vkEngine;
        vax::ResourceManager _resourceManager;
        vax::textures::TextureLoader _textureLoader;
        vax::objects::ModelLoader _modelLoader;
        vax::objects::PrimitivesBuilder _primitivesBuilder;
        UniformBufferObject _ubo;
        std::vector<vax::objects::DrawableModel> _drawableModels; // TODO: change to value type
    };
}