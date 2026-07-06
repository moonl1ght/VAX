#pragma once

#include "device.h"
#include "luna.h"

namespace vax::vk {
class RenderPassDescriptor final {
  public:
    VkFormat imageFormat;
    VkFormat depthFormat;

    explicit RenderPassDescriptor(
        const vax::vk::Device& device, VkRenderPass renderPass, VkFormat imageFormat, VkFormat depthFormat
    )
        : _device(device)
        , _renderPass(renderPass)
        , imageFormat(imageFormat)
        , depthFormat(depthFormat) {};

    RenderPassDescriptor(const RenderPassDescriptor& other) = delete;
    RenderPassDescriptor& operator=(const RenderPassDescriptor& other) = delete;

    RenderPassDescriptor(RenderPassDescriptor&& other) noexcept
        : _device(other._device)
        , _renderPass(other._renderPass)
        , imageFormat(other.imageFormat)
        , depthFormat(other.depthFormat) {
        other._renderPass = VK_NULL_HANDLE;
    }

    RenderPassDescriptor& operator=(RenderPassDescriptor&& other) noexcept {
        if (this != &other) {
            vkDestroyRenderPass(_device.get().vkDevice, _renderPass, nullptr);
            _device = other._device;
            _renderPass = other._renderPass;
            imageFormat = other.imageFormat;
            depthFormat = other.depthFormat;
            other._renderPass = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~RenderPassDescriptor() { vkDestroyRenderPass(_device.get().vkDevice, _renderPass, nullptr); };

    const VkRenderPass getVkRenderPass() const { return _renderPass; }

  private:
    std::reference_wrapper<const vax::vk::Device> _device;
    VkRenderPass _renderPass = VK_NULL_HANDLE;
};
} // namespace vax::vk