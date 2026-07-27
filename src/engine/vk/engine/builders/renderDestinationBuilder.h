#pragma once

#include "commandManager.h"
#include "device.h"
#include "luna.h"
#include "renderDestination.h"
#include "renderPassDescriptor.h"
#include "swapchain.h"

namespace vax::vk {
class RenderDestinationBuilder {
  public:
    explicit RenderDestinationBuilder(const Device& device, VmaAllocator allocator)
        : _device(device)
        , _allocator(allocator) {};

    std::optional<RenderDestination> buildMainSwapchain(
        CommandManager& commandManager,
        VkQueue submitQueue,
        Swapchain& swapchain,
        RenderPassDescriptor& renderPassDescriptor
    ) const noexcept;

    std::optional<RenderDestination> buildMainOffscreen(
        CommandManager& commandManager,
        VkQueue submitQueue,
        VkExtent2D extent,
        RenderPassDescriptor& renderPassDescriptor
    ) const noexcept;

    std::optional<RenderDestination> buildShadowSunOffscreen(
        CommandManager& commandManager,
        VkQueue submitQueue,
        VkExtent2D extent,
        RenderPassDescriptor& renderPassDescriptor
    ) const noexcept;

  private:
    vax::Logger _logger = vax::Logger("RenderDestinationBuilder");
    std::reference_wrapper<const Device> _device;
    VmaAllocator _allocator;
};
} // namespace vax::vk