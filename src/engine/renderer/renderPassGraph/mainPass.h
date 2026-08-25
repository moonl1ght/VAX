#pragma once

#include "renderPass_V2.h"

namespace vax::engine {
class MainPass : public RenderPass_V2 {
  public:
    MainPass(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        vax::vk::RenderDestination& renderDestination,
        vax::vk::RenderPassDescriptor& renderDescriptor
    )
        : RenderPass_V2(
              device, pipelineManager, descriptorSetManager, "MainPass", renderDestination, renderDescriptor
          ) {};

    MainPass(const MainPass&) = delete;
    MainPass(MainPass&&) = default;
    MainPass& operator=(const MainPass&) = delete;
    MainPass& operator=(MainPass&&) = default;

    ~MainPass() override = default;

    void runPass(RunPassInfo& runPassInfo) override;

  private:
    vax::Logger _logger = vax::Logger("MainPass");

    bool _drawGizmo(RunPassInfo& runPassInfo);
};
} // namespace vax::engine