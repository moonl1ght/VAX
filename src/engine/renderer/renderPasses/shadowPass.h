#pragma once

#include "renderPass_V2.h"

namespace vax::engine {
class ShadowPass : public RenderPass_V2 {
  public:
    ShadowPass(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        std::shared_ptr<vax::vk::RenderDestination> destination,
        std::shared_ptr<vax::vk::RenderPassDescriptor> renderDescriptor
    )
        : RenderPass_V2(device, pipelineManager, descriptorSetManager, "ShadowPass", destination, renderDescriptor) {};

    ~ShadowPass() = default;

    void runPass(RunPassInfo& runPassInfo) override;

  private:
};
} // namespace vax::engine