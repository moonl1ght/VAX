#pragma once

#include "device.h"
#include "luna.h"
#include "renderDestination.h"
#include "swapchain.h"
#include "vkEngine.h"

namespace vax::vk {
class RenderPass;

class RenderDestinationBuilder {
  public:
    explicit RenderDestinationBuilder(
        const vax::vk::Device& device,
        VmaAllocator allocator,
        const vax::vk::Swapchain& swapchain,
        const vax::vk::RenderPass& renderPass
    )
        : _device(device)
        , _allocator(allocator)
        , _swapchain(swapchain)
        , _renderPass(renderPass) {};

    std::optional<std::unique_ptr<vax::vk::RenderDestination>> build(vax::vk::Engine* vkEngine) const noexcept;

  private:
    vax::Logger _logger = vax::Logger("RenderDestinationBuilder");
    std::reference_wrapper<const vax::vk::Device> _device;
    VmaAllocator _allocator;
    std::reference_wrapper<const vax::vk::Swapchain> _swapchain;
    std::reference_wrapper<const vax::vk::RenderPass> _renderPass;

    bool createFramebuffers(
        const vax::textures::Texture& depthTexture, std::vector<VkFramebuffer>& swapchainFramebuffers
    ) const;
};
} // namespace vax::vk