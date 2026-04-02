#pragma once

#include "luna.h"

namespace vax {
    enum class PipelineType {
        RENDER,
        COMPUTE
    };

    class Pipeline final {
    protected:
        const vax::VkEngine& vkEngine;
    public:
        PipelineType pipelineType;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;

        Pipeline(
            const vax::VkEngine& vkEngine,
            PipelineType pipelineType,
            VkPipelineLayout pipelineLayout,
            VkPipeline pipeline
        ) :
            vkEngine(vkEngine),
            pipelineType(pipelineType),
            vkPipelineLayout(pipelineLayout),
            vkPipeline(pipeline) {
        };

        ~Pipeline() {
            vkDestroyPipelineLayout(vkEngine.device->vkDevice, vkPipelineLayout, nullptr);
            vkDestroyPipeline(vkEngine.device->vkDevice, vkPipeline, nullptr);
        };
    };
}
