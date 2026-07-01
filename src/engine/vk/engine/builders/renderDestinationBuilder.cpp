#include "renderDestinationBuilder.h"
#include "renderPass.h"
#include "textureFactory.h"
#include "textureTaskScheduler.h"

using namespace vax::vk;
using namespace vax;

std::optional<std::unique_ptr<RenderDestination>> RenderDestinationBuilder::build(Engine* vkEngine) const noexcept {
    _logger.info("Building render destination...");
    VkFormat depthFormat = findDepthFormat(_device.get().vkPhysicalDevice);

    auto depthTexture = TextureFactory(_device.get(), _allocator)
                            .makeDepthTextureDetached(depthFormat, math::SizeUI(_swapchain.get().swapchainExtent));

    if (!depthTexture.has_value()) {
        return std::nullopt;
    }

    auto textureTaskScheduler = TextureTaskScheduler(_device.get(), *vkEngine->commandManager);
    textureTaskScheduler.transitionTextureLayoutAndSubmit(
        vkEngine->queueManager->graphicsQueue,
        *depthTexture,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    depthTexture->loadImageView(VK_IMAGE_VIEW_TYPE_2D, 1, 1);

    std::vector<VkFramebuffer> swapchainFramebuffers;
    if (!createFramebuffers(*depthTexture, swapchainFramebuffers)) {
        return std::nullopt;
    }

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto drawImage = TextureFactory(_device.get(), _allocator)
                         .makeTextureDetached(
                             TextureFactory::TextureCreateInfo{
                                 .name = "draw_image",
                                 .format = VK_FORMAT_R16G16B16A16_SFLOAT,
                                 .size = vax::math::SizeUI(_swapchain.get().swapchainExtent),
                                 .imageUsageFlags = drawImageUsages
                             }
                         );
    if (!drawImage.has_value()) {
        return std::nullopt;
    }

    return std::make_optional<std::unique_ptr<vax::vk::RenderDestination>>(std::make_unique<vax::vk::RenderDestination>(
        _device.get(),
        std::make_unique<vax::vk::Texture>(std::move(*depthTexture)),
        std::make_unique<vax::vk::Texture>(std::move(*drawImage)),
        std::move(swapchainFramebuffers)
    ));
}

bool RenderDestinationBuilder::createFramebuffers(
    const Texture& depthTexture, std::vector<VkFramebuffer>& swapchainFramebuffers
) const {
    swapchainFramebuffers.resize(_swapchain.get().swapchainImageViews.size());

    for (size_t i = 0; i < _swapchain.get().swapchainImageViews.size(); ++i) {
        std::array<VkImageView, 2> attachments = {_swapchain.get().swapchainImageViews[i], depthTexture.imageView()};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass.get().getVkRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapchain.get().swapchainExtent.width;
        framebufferInfo.height = _swapchain.get().swapchainExtent.height;
        framebufferInfo.layers = 1;

        auto result = vkCreateFramebuffer(_device.get().vkDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]);
        if (!VK_CHECK(result)) {
            _logger.error("Failed to create framebuffer!");
            return false;
        }
    }

    return true;
}