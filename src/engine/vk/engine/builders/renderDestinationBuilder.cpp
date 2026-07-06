#include "renderDestinationBuilder.h"
#include "textureFactory.h"
#include "textureTaskScheduler.h"

using namespace vax::vk;
using namespace vax;

std::optional<RenderDestination> RenderDestinationBuilder::build(
    CommandManager& commandManager,
    VkQueue submitQueue,
    Swapchain& swapchain,
    RenderPassDescriptor& renderPassDescriptor
) const noexcept {
    _logger.info("Building render destination...");
    VkFormat depthFormat = renderPassDescriptor.depthFormat;
    auto depthTexture = TextureFactory(_device.get(), _allocator)
                            .makeDepthTextureDetached(depthFormat, math::SizeUI(swapchain.swapchainExtent));

    if (!depthTexture.has_value()) {
        return std::nullopt;
    }

    auto textureTaskScheduler = TextureTaskScheduler(_device.get(), commandManager);
    textureTaskScheduler.transitionTextureLayoutAndSubmit(
        submitQueue,
        *depthTexture,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    depthTexture->loadImageView(VK_IMAGE_VIEW_TYPE_2D, 1, 1);

    std::vector<VkFramebuffer> swapchainFramebuffers;
    swapchainFramebuffers.resize(swapchain.swapchainImageViews.size());

    for (size_t i = 0; i < swapchain.swapchainImageViews.size(); ++i) {
        std::array<VkImageView, 2> attachments = {swapchain.swapchainImageViews[i], depthTexture->imageView()};

        VkFramebufferCreateInfo framebufferInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPassDescriptor.getVkRenderPass(),
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .width = swapchain.swapchainExtent.width,
            .height = swapchain.swapchainExtent.height,
            .layers = 1,
        };
        auto result = vkCreateFramebuffer(_device.get().vkDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]);
        if (!VK_CHECK(result)) {
            _logger.error("Failed to create framebuffer!");
            return std::nullopt;
        }
    }

    // VkImageUsageFlags drawImageUsages{};
    // drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    // drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    // drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // auto drawImage = TextureFactory(_device.get(), _allocator)
    //                      .makeTextureDetached(
    //                          TextureFactory::TextureCreateInfo{
    //                          .name = "draw_image",
    //                          .format = VK_FORMAT_R16G16B16A16_SFLOAT,
    //                          .size = vax::math::SizeUI(swapchain.swapchainExtent),
    //                          .imageUsageFlags = drawImageUsages
    //                          }
    //                      );
    // if (!drawImage.has_value()) {
    //     return std::nullopt;
    // }

    // std::vector<vax::vk::Texture> mainOffscreenTextures;

    return std::make_optional<RenderDestination>(
        _device.get(),
        std::make_unique<Texture>(std::move(*depthTexture)),
        std::vector<Texture>{},
        std::move(swapchainFramebuffers)
    );
}