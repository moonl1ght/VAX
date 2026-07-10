#pragma once

#include "device.h"
#include "luna.h"
#include "pipeline.h"

namespace vax::engine {
class RenderPass final {
  public:
    RenderPass(
        vax::vk::Device& device,
        std::string_view name,
        VkRenderPass renderPass,
        VkFramebuffer framebuffer,
        VkExtent2D extent,
        uint32_t colorAttachmentCount = 1
    ) noexcept {
        if (!name.empty()) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VK_OBJECT_TYPE_RENDER_PASS,
                .objectHandle = reinterpret_cast<size_t>(renderPass),
                .pObjectName = name.data(),
            };
            vax::vk::pfnSetDebugUtilsObjectNameEXT(device.vkDevice, &nameInfo);
        }
        _clearValues.assign(colorAttachmentCount, VkClearValue{.color = {{0.0f, 0.0f, 0.0f, 0.0f}}});
        _clearValues.push_back(VkClearValue{.depthStencil = {1.0f, 0}});
        _renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = framebuffer,
            .renderArea = {.offset = {0, 0}, .extent = extent},
            .clearValueCount = static_cast<uint32_t>(_clearValues.size()),
            .pClearValues = _clearValues.data()
        };
    };

    explicit RenderPass(vax::vk::Device& device, std::string_view name, VkRenderPassBeginInfo renderPassInfo) noexcept
        : _renderPassInfo(renderPassInfo) {
        if (!name.empty()) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VK_OBJECT_TYPE_RENDER_PASS,
                .objectHandle = reinterpret_cast<size_t>(renderPassInfo.renderPass),
                .pObjectName = name.data(),
            };
            vax::vk::pfnSetDebugUtilsObjectNameEXT(device.vkDevice, &nameInfo);
        }
    };

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
    std::vector<VkClearValue> _clearValues;
};
} // namespace vax::engine