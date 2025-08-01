#ifndef PipelineManager_hpp
#define PipelineManager_hpp

#include "luna.h"
#include "VKObject.hpp"
#include "Vertex.h"
#include "DescriptorSetManager.hpp"

class PipelineManager final : public VKObject {
public:
    PipelineManager(VKEngine* engine, DescriptorSetManager* descriptorSetManager)
        : VKObject(engine)
        , _descriptorSetManager(descriptorSetManager) {
    };

    ~PipelineManager() {
        vkDestroyPipelineLayout(vkEngine->device->vkDevice, _pipelineLayout, nullptr);
        vkDestroyPipeline(vkEngine->device->vkDevice, _pipeline, nullptr);
    };

    bool setup();
    VkPipeline getPipeline() const { return _pipeline; }
    VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }

private:
    DescriptorSetManager* _descriptorSetManager;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;
};

#endif