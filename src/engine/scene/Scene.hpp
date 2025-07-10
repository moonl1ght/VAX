#ifndef Scene_hpp
#define Scene_hpp

#include "luna.h"
#include "DrawableModel.hpp"
#include "VKStack.hpp"
#include "Texture.hpp"
#include "ShaderUniforms.h"

class Scene {
public:
    Texture* texture = nullptr;

    Scene(VKStack* vkStack) : _vkStack(vkStack) { };
    ~Scene() {
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
    VKStack* _vkStack;

    UniformBufferObject _ubo;
    std::vector<DrawableModel*> _drawableModels; // TODO: change to value type
};

#endif