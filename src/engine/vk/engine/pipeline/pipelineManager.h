#pragma once

#include "Vertex.h"
#include "descriptorSetManager.h"
#include "device.h"
#include "luna.h"
#include "pipeline.h"

namespace vax::vk {
    class RenderPass;
}

namespace vax::vk {
    class PipelineManager final {
    public:
        PipelineManager(
            const vax::vk::Device& device,
            const vax::vk::DescriptorSetManager& descriptorSetManager)
            : _device(device), _descriptorSetManager(descriptorSetManager) {
        };

        ~PipelineManager() {
            vkDestroyPipelineLayout(_device.get().vkDevice, _pipelineLayout, nullptr);
            vkDestroyPipeline(_device.get().vkDevice, _pipeline, nullptr);
        };

        PipelineManager(const PipelineManager&) = delete;
        PipelineManager& operator=(const PipelineManager&) = delete;
        PipelineManager(PipelineManager&& other) = delete;
        PipelineManager& operator=(PipelineManager&& other) = delete;

        bool setup(const vax::vk::RenderPass& renderPass);

        VkPipeline getPipeline() const { return _pipeline; }
        VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }
        const vax::vk::Pipeline& getBackgroundPipeline() const {
            if (!_backgroundPipeline) {
                throw std::runtime_error("Background pipeline not found!");
            }
            return *_backgroundPipeline;
        };

    private:
        vax::utils::Logger _logger = vax::utils::Logger("PipelineManager");
        std::reference_wrapper<const vax::vk::Device> _device;
        std::reference_wrapper<const vax::vk::DescriptorSetManager> _descriptorSetManager;

        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
        VkPipeline _pipeline = VK_NULL_HANDLE;

        std::unique_ptr<vax::vk::Pipeline> _backgroundPipeline = nullptr;
    };
}