#pragma once

#include "vertex.h"
#include "descriptorSetManager.h"
#include "device.h"
#include "luna.h"
#include "pipeline.h"
#include "shaderModuleBuilder.h"

namespace vax::vk {
    class RenderPass;
}

namespace vax::vk {
    class PipelineManager final {
    public:
        PipelineManager(
            const vax::vk::Device& device,
            const vax::vk::DescriptorSetManager& descriptorSetManager)
            : _device(device)
            , _descriptorSetManager(descriptorSetManager)
            , _shaderModuleBuilder(device) {
        };

        ~PipelineManager() {};

        PipelineManager(const PipelineManager&) = delete;
        PipelineManager& operator=(const PipelineManager&) = delete;
        PipelineManager(PipelineManager&& other) = delete;
        PipelineManager& operator=(PipelineManager&& other) = delete;

        bool setup(const vax::vk::RenderPass& renderPass);

        const vax::vk::Pipeline* getPipeline(vax::vk::PipelineName pipelineName) const;

    private:
        vax::utils::Logger _logger = vax::utils::Logger("PipelineManager");
        std::reference_wrapper<const vax::vk::Device> _device;
        std::reference_wrapper<const vax::vk::DescriptorSetManager> _descriptorSetManager;
        vax::vk::ShaderModuleBuilder _shaderModuleBuilder;

        std::unordered_map<std::string, vax::vk::Pipeline> _pipelines;

        bool _createPBRPipeline(const vax::vk::RenderPass& renderPass);
        bool _createBackgroundPipeline(const vax::vk::RenderPass& renderPass);
    };
}