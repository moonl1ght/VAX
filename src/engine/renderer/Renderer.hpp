#ifndef Renderer_hpp
#define Renderer_hpp

#include "luna.h"
#include "VKEngine.hpp"
#include "DescriptorSetManager.hpp"
#include "PipelineManager.hpp"
#include "Scene.hpp"
#include "Buffer.hpp"

class Renderer {
public:
    Renderer(
        VKEngine* vkEngine, 
        PipelineManager* pipelineManager,
        DescriptorSetManager* descriptorSetManager
    )
    : _vkEngine(vkEngine)
    , _pipelineManager(pipelineManager)
    , _descriptorSetManager(descriptorSetManager) {};

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
    VKEngine* _vkEngine;
    PipelineManager* _pipelineManager;
    DescriptorSetManager* _descriptorSetManager;

    std::vector<Buffer*> _sceneUniformBuffers;
    std::vector<void*> _sceneUniformBuffersMapped;

    uint32_t _currentFrame = 0;

    bool recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene, float deltaTime);
};

#endif