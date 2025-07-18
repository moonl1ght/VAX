#ifndef PipelineManager_hpp
#define PipelineManager_hpp

#include "luna.h"
#include "VKObject.hpp"
#include "Vertex.h"
#include "DescriptorSetManager.hpp"

class PipelineManager final : public VKObject {
public:
    PipelineManager(VKStack* stack, DescriptorSetManager* descriptorSetManager)
        : VKObject(stack)
        , _descriptorSetManager(descriptorSetManager) {
    };

    ~PipelineManager() {
        vkDestroyPipelineLayout(stack->device->vkDevice, _pipelineLayout, nullptr);
        vkDestroyPipeline(stack->device->vkDevice, _pipeline, nullptr);
    };

    bool initialize();
    VkPipeline getPipeline() const { return _pipeline; }
    VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }

private:
    DescriptorSetManager* _descriptorSetManager;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;
};

#endif