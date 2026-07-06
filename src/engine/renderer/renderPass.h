#pragma once

#include "device.h"
#include "luna.h"
#include "pipeline.h"

namespace vax::engine {
class RenderPass final {
  public:
    explicit RenderPass(VkRenderPassBeginInfo renderPassInfo)
        : _renderPassInfo(renderPassInfo) {};

    RenderPass(const RenderPass& other) = delete;
    RenderPass& operator=(const RenderPass& other) = delete;
    RenderPass(RenderPass&& other) noexcept = delete;
    RenderPass& operator=(RenderPass&& other) noexcept = delete;

    ~RenderPass() {};

    template <typename Work> void pass(VkCommandBuffer commandBuffer, Work work) {
        vkCmdBeginRenderPass(commandBuffer, &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        work();
        vkCmdEndRenderPass(commandBuffer);
    }

  private:
    vax::Logger _logger = vax::Logger("RenderPass");
    VkRenderPassBeginInfo _renderPassInfo;
};
} // namespace vax::engine