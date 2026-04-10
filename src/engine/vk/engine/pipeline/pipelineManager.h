#pragma once

#include "luna.h"
#include "vkObject.h"
#include "Vertex.h"
#include "DescriptorSetManager.hpp"
#include "pipeline.h"

namespace vax {
    class PipelineManager final : public vax::VkObject {
    public:
        PipelineManager(vax::VkEngine* vkEngine, DescriptorSetManager* descriptorSetManager)
            : vax::VkObject(vkEngine)
            , _descriptorSetManager(descriptorSetManager) {
        };

        ~PipelineManager() {
            vkDestroyPipelineLayout(vkEngine->device->vkDevice, _pipelineLayout, nullptr);
            vkDestroyPipeline(vkEngine->device->vkDevice, _pipeline, nullptr);
        };

        PipelineManager(const PipelineManager&) = delete;
        PipelineManager& operator=(const PipelineManager&) = delete;

        PipelineManager(PipelineManager&& other) noexcept
            : vax::VkObject(other.vkEngine)
            , _descriptorSetManager(other._descriptorSetManager)
            , _pipelineLayout(other._pipelineLayout)
            , _pipeline(other._pipeline)
            , _backgroundPipeline(std::move(other._backgroundPipeline)) {
            other._descriptorSetManager = nullptr;
            other._pipelineLayout = VK_NULL_HANDLE;
            other._pipeline = VK_NULL_HANDLE;
            other._backgroundPipeline = nullptr;
        };

        PipelineManager& operator=(PipelineManager&& other) noexcept {
            if (this != &other) {
                std::swap(_descriptorSetManager, other._descriptorSetManager);
                std::swap(_pipelineLayout, other._pipelineLayout);
                std::swap(_pipeline, other._pipeline);
                // std::swap(_backgroundPipeline, other._backgroundPipeline);
            }
            return *this;
        };

        bool setup();
        VkPipeline getPipeline() const { return _pipeline; }
        VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }
        const vax::vk::Pipeline& getBackgroundPipeline() const {
            if (!_backgroundPipeline) {
                throw std::runtime_error("Background pipeline not found!");
            }
            return *_backgroundPipeline;
        };

    private:
        Logger _logger = Logger("PipelineManager");
        DescriptorSetManager* _descriptorSetManager;
        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
        VkPipeline _pipeline = VK_NULL_HANDLE;

        std::unique_ptr<vax::vk::Pipeline> _backgroundPipeline = nullptr;
    };
}