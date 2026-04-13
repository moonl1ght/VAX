#ifndef Scene_hpp
#define Scene_hpp

#include "luna.h"
#include "DrawableModel.hpp"
#include "texture.h"
#include "ShaderUniforms.h"
#include "ModelLoader.hpp"
#include "vkObject.h"

class Scene final : public vax::VkObject {
public:
    vax::textures::Texture* texture = nullptr;

    Scene(vax::vk::Engine* vkEngine)
        : vax::VkObject(vkEngine)
        , _modelLoader(ModelLoader(vkEngine)) {
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

    const std::vector<DrawableModel*>& getDrawableModels() const {
        return _drawableModels;
    }

    const UniformBufferObject& getUBO() const {
        return _ubo;
    }

private:
    ModelLoader _modelLoader;
    UniformBufferObject _ubo;
    std::vector<DrawableModel*> _drawableModels; // TODO: change to value type
};

#endif