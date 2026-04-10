#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    enum class PipelineType {
        RENDER,
        COMPUTE,
        UNKNOWN
    };

    class Pipeline final {
    public:
        PipelineType pipelineType = PipelineType::UNKNOWN;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;

        Pipeline(const Pipeline& other) = delete;
        Pipeline& operator=(const Pipeline& other) = delete;
        Pipeline(Pipeline&& other) = delete;
        Pipeline& operator=(Pipeline&& other) = delete;

        explicit Pipeline(
            const vax::vk::Device& device,
            PipelineType pipelineType,
            VkPipelineLayout pipelineLayout,
            VkPipeline pipeline
        ) noexcept
            : _device(device)
            , pipelineType(pipelineType)
            , vkPipelineLayout(pipelineLayout)
            , vkPipeline(pipeline) {
        };

        ~Pipeline() {
            vkDestroyPipelineLayout(_device.get().vkDevice, vkPipelineLayout, nullptr);
            vkDestroyPipeline(_device.get().vkDevice, vkPipeline, nullptr);
        };

    private:
        Logger _logger = Logger("Pipeline");
        std::reference_wrapper<const vax::vk::Device> _device;
    };
}
