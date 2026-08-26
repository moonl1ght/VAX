#pragma once

#include "descriptorSetManager.h"
#include "device.h"
#include "logger.h"
#include "pipelineManager.h"
#include "renderDestination.h"
#include <unordered_map>

namespace vax::engine {
class RenderPassGraphFactory final {
  public:
    RenderPassGraphFactory(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager
    )
        : _device(device)
        , _pipelineManager(pipelineManager)
        , _descriptorSetManager(descriptorSetManager) {};

    ~RenderPassGraphFactory() = default;

    void setupRenderPassDescriptors(VkFormat imageFormat);

    void buildRoverDemoGraph();

  private:
    vax::Logger _logger = vax::Logger("rednerPassGraphFactory");
    std::reference_wrapper<vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::PipelineManager> _pipelineManager;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;

    std::unordered_map<std::string, vax::vk::RenderPassDescriptor> _renderPassDescriptors;
    std::unordered_map<std::string, vax::vk::RenderDestination> _renderDestinations;
};
} // namespace vax::engine