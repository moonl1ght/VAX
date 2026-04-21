#ifndef Renderer_hpp
#define Renderer_hpp

#include "luna.h"
#include "vkEngine.h"
#include "scene.h"
#include "buffer.h"

class Renderer {
public:
    Renderer(
        vax::vk::Engine* vkEngine
    )
        : _vkEngine(vkEngine) {
    };

    ~Renderer() {
        _sceneUniformBuffers.clear();
        _sceneUniformBuffersMapped.clear();
    };

    bool render(vax::Scene* scene, float deltaTime);
    void prepare();

private:
    vax::vk::Engine* _vkEngine;

    std::vector<vax::vk::Buffer> _sceneUniformBuffers;
    std::vector<void*> _sceneUniformBuffersMapped;

    uint32_t _currentFrame = 0;

    void drawBackground(VkCommandBuffer commandBuffer);
    bool recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::Scene* scene, float deltaTime);
};

#endif