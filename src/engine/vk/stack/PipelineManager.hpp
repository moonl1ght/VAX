#ifndef PipelineManager_hpp
#define PipelineManager_hpp

#include "luna.h"
#include "VKStack.hpp"
#include "DescriptorSetManager.hpp"

class PipelineManager {
public:
    PipelineManager(VKStack* vkStack, DescriptorSetManager* descriptorSetManager)
    : _vkStack(vkStack)
    , _descriptorSetManager(descriptorSetManager) { };
    ~PipelineManager() {
        vkDestroyPipelineLayout(_vkStack->device->vkDevice, _pipelineLayout, nullptr);
        vkDestroyPipeline(_vkStack->device->vkDevice, _pipeline, nullptr);
    };

    bool initialize();
    VkPipeline getPipeline() const { return _pipeline; }
    VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }

private:
    VKStack* _vkStack;
    DescriptorSetManager* _descriptorSetManager;

    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;
};

#endif