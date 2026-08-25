#pragma once

#include "descriptorSetManager.h"
#include "device.h"
#include "pipelineManager.h"

namespace vax::engine {
class RenderPassBuilder final {
  public:
    RenderPassBuilder(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager
    )
        : _device(device)
        , _pipelineManager(pipelineManager)
        , _descriptorSetManager(descriptorSetManager) {};

    ~RenderPassBuilder() = default;

  private:
    std::reference_wrapper<vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::PipelineManager> _pipelineManager;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;
};
} // namespace vax::engine