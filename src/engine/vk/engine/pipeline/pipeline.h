#pragma once

#include "luna.h"
#include "vkObject.h"

namespace vax {
    enum class PipelineType {
        RENDER,
        COMPUTE,
        UNKNOWN
    };

    class Pipeline final : public vax::VkObject { 
    public:
        PipelineType pipelineType = PipelineType::UNKNOWN;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;

        Pipeline(vax::VkEngine* vkEngine) : vax::VkObject(vkEngine) {};

        Pipeline(const Pipeline& other) = delete;

        Pipeline& operator=(const Pipeline& other) = delete;

        Pipeline(Pipeline&& other) : vax::VkObject(other.vkEngine) {
            std::swap(pipelineType, other.pipelineType);
            std::swap(vkPipelineLayout, other.vkPipelineLayout);
            std::swap(vkPipeline, other.vkPipeline);
        };

        Pipeline& operator=(Pipeline&& other) noexcept {
            if (this != &other) {
                std::swap(vkEngine, other.vkEngine);
                std::swap(pipelineType, other.pipelineType);
                std::swap(vkPipelineLayout, other.vkPipelineLayout);
                std::swap(vkPipeline, other.vkPipeline);
            }
            return *this;
        }

        Pipeline(
            vax::VkEngine* vkEngine,
            PipelineType pipelineType,
            VkPipelineLayout pipelineLayout,
            VkPipeline pipeline
        ) :
            vax::VkObject(vkEngine),
            pipelineType(pipelineType),
            vkPipelineLayout(pipelineLayout),
            vkPipeline(pipeline) {
        };

        ~Pipeline() {
            vkDestroyPipelineLayout(vkEngine->device->vkDevice, vkPipelineLayout, nullptr);
            vkDestroyPipeline(vkEngine->device->vkDevice, vkPipeline, nullptr);
        };
    };
}
