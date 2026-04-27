#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "texture.h"
#include "shaderUniforms.h"
#include "modelLoader.h"
#include "primitivesBuilder.h"
#include "vkEngine.h"

namespace vax {
    class Scene final {
    public:
        vax::textures::Texture* texture = nullptr;

        Scene(vax::vk::Engine& vkEngine)
            : _vkEngine(vkEngine)
            , _modelLoader(vax::objects::ModelLoader(&vkEngine))
            , _primitivesBuilder(vax::objects::PrimitivesBuilder(vkEngine.resourceManager->meshManager())) {
        };

        ~Scene() {
            delete texture;
            texture = nullptr;
            _drawableModels.clear();
        };

        Scene(const Scene& other) = delete;
        Scene& operator=(const Scene& other) = delete;
        Scene(Scene&& other) noexcept = delete;
        Scene& operator=(Scene&& other) noexcept = delete;

        void load();
        void update(float deltaTime);

        std::vector<vax::objects::DrawableModel>& getDrawableModels() {
            return _drawableModels;
        }

        const UniformBufferObject& getUBO() const {
            return _ubo;
        }

    private:
        std::reference_wrapper<vax::vk::Engine> _vkEngine;
        vax::objects::ModelLoader _modelLoader;
        vax::objects::PrimitivesBuilder _primitivesBuilder;
        UniformBufferObject _ubo;
        std::vector<vax::objects::DrawableModel> _drawableModels; // TODO: change to value type
    };
}