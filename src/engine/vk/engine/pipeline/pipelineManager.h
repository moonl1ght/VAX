#pragma once

#include "descriptorSetManager.h"
#include "device.h"
#include "luna.h"
#include "pipeline.h"
#include "shaderModuleBuilder.h"
#include "renderPassDescriptor.h"
#include "vertex.h"

namespace vax::vk {
class PipelineManager final {
  public:
    PipelineManager(const Device& device, const DescriptorSetManager& descriptorSetManager)
        : _device(device)
        , _descriptorSetManager(descriptorSetManager)
        , _shaderModuleBuilder(device) {};

    ~PipelineManager() {
        for (auto& [name, pipelineLayout] : _pipelineLayouts) {
            vkDestroyPipelineLayout(_device.get().vkDevice, pipelineLayout, nullptr);
        }
    };

    PipelineManager(const PipelineManager&) = delete;
    PipelineManager& operator=(const PipelineManager&) = delete;
    PipelineManager(PipelineManager&& other) = delete;
    PipelineManager& operator=(PipelineManager&& other) = delete;

    bool setup(const RenderPassDescriptor& renderPassDescriptor);

    const Pipeline* getPipeline(PipelineName pipelineName) const;
    VkPipelineLayout getPipelineLayout(PipelineLayoutName pipelineLayoutName) const;

  private:
    vax::Logger _logger = vax::Logger("PipelineManager");
    std::reference_wrapper<const Device> _device;
    std::reference_wrapper<const DescriptorSetManager> _descriptorSetManager;
    ShaderModuleBuilder _shaderModuleBuilder;

    std::unordered_map<std::string, Pipeline> _pipelines;
    std::unordered_map<std::string, VkPipelineLayout> _pipelineLayouts;

    bool _createBackgroundPipelineLayout(PipelineLayoutName pipelineLayoutName);
    bool _createBasePipelineLayout(PipelineLayoutName pipelineLayoutName);

    bool _createPBRPipeline(const RenderPassDescriptor& renderPassDescriptor);
    bool _createBackgroundPipeline(const RenderPassDescriptor& renderPassDescriptor);
    bool _createBasePipeline(const RenderPassDescriptor& renderPassDescriptor);
};
} // namespace vax::vk