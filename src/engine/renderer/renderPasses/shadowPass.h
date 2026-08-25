#pragma once

#include "renderPass_V2.h"

namespace vax::engine {
class ShadowPass : public RenderPass_V2 {
  public:
    ShadowPass(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        vax::vk::RenderDestination& renderDestination,
        vax::vk::RenderPassDescriptor& renderDescriptor
    )
        : RenderPass_V2(
              device, pipelineManager, descriptorSetManager, "ShadowPass", renderDestination, renderDescriptor
          ) {};

    ShadowPass(const ShadowPass&) = delete;
    ShadowPass(ShadowPass&&) = default;
    ShadowPass& operator=(const ShadowPass&) = delete;
    ShadowPass& operator=(ShadowPass&&) = default;

    ~ShadowPass() override = default;

    void runPass(RunPassInfo& runPassInfo) override;

  private:
    vax::Logger _logger = vax::Logger("ShadowPass");
};
} // namespace vax::engine