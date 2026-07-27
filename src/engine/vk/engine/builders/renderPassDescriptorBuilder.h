#pragma once

#include "device.h"
#include "renderPassDescriptor.h"

namespace vax::vk {
class RenderPassDescriptorBuilder {
  public:
    explicit RenderPassDescriptorBuilder(const vax::vk::Device& device)
        : _device(device) {};

    std::optional<vax::vk::RenderPassDescriptor> buildMainSwapchain(VkFormat imageFormat) const noexcept;

    std::optional<vax::vk::RenderPassDescriptor>
    buildMainOffscreen(VkFormat imageFormat, bool allowComputeUsage) const noexcept;

    std::optional<vax::vk::RenderPassDescriptor>
    buildShadowSun(VkFormat imageFormat, bool allowComputeUsage) const noexcept;

  private:
    vax::Logger _logger = vax::Logger("RenderPassDescriptorBuilder");
    std::reference_wrapper<const vax::vk::Device> _device;
};
} // namespace vax::vk