#include "renderDestinationBuilder.h"
#include "textureFactory.h"
#include "renderPass.h"

using namespace vax::vk;
using namespace vax;

std::optional<std::unique_ptr<RenderDestination>> RenderDestinationBuilder::build(
    Engine* vkEngine
) const noexcept {
    _logger.info("Building render destination...");
    VkFormat depthFormat = utils::findDepthFormat(_device.get().vkPhysicalDevice);

    // TODO: refactor this
    // vk::CommandBuffer commandBuffer = vkEngine->commandManager->createSingleTimeCommandBuffer();
    auto depthTexture = textures::TextureFactory(
        _device.get(), _allocator
    )
        .makeDepthTexture(depthFormat, math::SizeUI(_swapchain.get().swapchainExtent), *vkEngine->commandManager, vkEngine->queueManager->graphicsQueue);

    std::cout << "depthTexture: " << depthTexture.has_value() << std::endl;

    if (!depthTexture.has_value()) {
        return std::nullopt;
    }

    // depthTexture->loadImageView();

    std::vector<VkFramebuffer> swapchainFramebuffers;
    if (!createFramebuffers(*depthTexture, swapchainFramebuffers)) {
        return std::nullopt;
    }


    std::cout << "depthTexture: " << depthTexture.has_value() << std::endl;


    auto drawImage = textures::TextureFactory(_device.get(), _allocator)
        .makeTexture(VK_FORMAT_R16G16B16A16_SFLOAT, vax::math::SizeUI(_swapchain.get().swapchainExtent));
    if (!drawImage.has_value()) {
        return std::nullopt;
    }

    std::cout << "drawImage: " << std::endl;

    return std::make_optional<std::unique_ptr<vax::vk::RenderDestination>>(
        std::make_unique<vax::vk::RenderDestination>(
            _device.get(),
            std::make_unique<vax::textures::Texture>(std::move(*depthTexture)),
            std::make_unique<vax::textures::Texture>(std::move(*drawImage)),
            std::move(swapchainFramebuffers)
        )
    );
}

bool RenderDestinationBuilder::createFramebuffers(
    const textures::Texture& depthTexture,
    std::vector<VkFramebuffer>& swapchainFramebuffers
) const {
    swapchainFramebuffers.resize(_swapchain.get().swapchainImageViews.size());

    for (size_t i = 0; i < _swapchain.get().swapchainImageViews.size(); ++i) {
        std::array<VkImageView, 2> attachments = {
            _swapchain.get().swapchainImageViews[i],
            depthTexture.imageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass.get().getVkRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapchain.get().swapchainExtent.width;
        framebufferInfo.height = _swapchain.get().swapchainExtent.height;
        framebufferInfo.layers = 1;

        auto result = vkCreateFramebuffer(
            _device.get().vkDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]
        );
        if (!VK_CHECK(result)) {
            _logger.error("Failed to create framebuffer!");
            return false;
        }
    }

    return true;
}