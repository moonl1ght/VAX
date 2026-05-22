#pragma once

#include "device.h"
#include "luna.h"
#include "pipeline.h"

namespace vax::renderer {
class RendererPass final {
  public:
    explicit RendererPass(VkRenderPassBeginInfo renderPassInfo)
        : _renderPassInfo(renderPassInfo) {};

    RendererPass(const RendererPass& other) = delete;
    RendererPass& operator=(const RendererPass& other) = delete;
    RendererPass(RendererPass&& other) noexcept = delete;
    RendererPass& operator=(RendererPass&& other) noexcept = delete;

    ~RendererPass() {};

    template <typename Work> void pass(VkCommandBuffer commandBuffer, Work work) {
        vkCmdBeginRenderPass(commandBuffer, &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        work();
        vkCmdEndRenderPass(commandBuffer);
    }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("RendererPass");
    VkRenderPassBeginInfo _renderPassInfo;
};
} // namespace vax::renderer