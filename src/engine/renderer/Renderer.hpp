#ifndef Renderer_hpp
#define Renderer_hpp

#include "luna.h"
#include "vkEngine.h"
#include "Scene.hpp"
#include "Buffer.hpp"

class Renderer {
public:
    Renderer(
        vax::VkEngine* vkEngine
    )
        : _vkEngine(vkEngine) {
    };

    ~Renderer() {
        for (size_t i = 0; i < _vkEngine->MAX_FRAMES_IN_FLIGHT; i++) {
            delete _sceneUniformBuffers[i];
            _sceneUniformBuffers[i] = nullptr;
        }
        _sceneUniformBuffers.clear();
        _sceneUniformBuffersMapped.clear();
    };

    bool render(Scene* scene, float deltaTime);
    void prepare();

private:
    vax::VkEngine* _vkEngine;

    std::vector<Buffer*> _sceneUniformBuffers;
    std::vector<void*> _sceneUniformBuffersMapped;

    uint32_t _currentFrame = 0;

    void drawBackground(VkCommandBuffer commandBuffer);
    bool recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene, float deltaTime);
};

#endif