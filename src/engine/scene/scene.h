#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "texture.h"
#include "shaderUniforms.h"
#include "modelLoader.h"
#include "vkObject.h"

namespace vax {
    class Scene final : public vax::VkObject {
    public:
        vax::textures::Texture* texture = nullptr;

        Scene(vax::vk::Engine* vkEngine)
            : vax::VkObject(vkEngine)
            , _modelLoader(vax::objects::ModelLoader(vkEngine)) {
        };

        ~Scene() {
            delete texture;
            texture = nullptr;
            for (auto model : _drawableModels) {
                delete model;
            }
            _drawableModels.clear();
        };

        void load();
        void update(float deltaTime);

        const std::vector<vax::objects::DrawableModel*>& getDrawableModels() const {
            return _drawableModels;
        }

        const UniformBufferObject& getUBO() const {
            return _ubo;
        }

    private:
        vax::objects::ModelLoader _modelLoader;
        UniformBufferObject _ubo;
        std::vector<vax::objects::DrawableModel*> _drawableModels; // TODO: change to value type
    };
}