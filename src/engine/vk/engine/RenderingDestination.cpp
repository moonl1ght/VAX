#include "RenderingDestination.hpp"
#include "vkEngine.h"
#include "ImageUtils.hpp"
#include "Texture.hpp"

bool RenderingDestination::setup() {
    if (!createDepthResources()) return false;
    if (!createFramebuffers()) return false;
    if (!createRenderDesctinationTexture(_swapchainManager->swapchainExtent)) return false;
    return true;
}

void RenderingDestination::cleanup() {
    for (auto framebuffer : swapchainFramebuffers) {
        vkDestroyFramebuffer(_vkEngine->device->vkDevice, framebuffer, nullptr);
    }

    delete depthTexture;
    depthTexture = nullptr;
    drawImage = nullptr;
}

void RenderingDestination::recreate() {
    int width = 0, height = 0;
    SDL_GetWindowSizeInPixels(_vkEngine->window, &width, &height);
    while (width == 0 || height == 0) {
        SDL_GetWindowSizeInPixels(_vkEngine->window, &width, &height);
        SDL_Delay(100);
    }
    vkDeviceWaitIdle(_vkEngine->device->vkDevice);

    cleanup();
    _swapchainManager->recreate();
    if (!createDepthResources()) return;
    if (!createFramebuffers()) return;
    if (!createRenderDesctinationTexture(_swapchainManager->swapchainExtent)) return;
}

bool RenderingDestination::createFramebuffers() {
    swapchainFramebuffers.resize(_swapchainManager->swapchainImageViews.size());

    for (size_t i = 0; i < _swapchainManager->swapchainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            _swapchainManager->swapchainImageViews[i],
            depthTexture->textureImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPassManager->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapchainManager->swapchainExtent.width;
        framebufferInfo.height = _swapchainManager->swapchainExtent.height;
        framebufferInfo.layers = 1;

        auto result = vkCreateFramebuffer(
            _vkEngine->device->vkDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]
        );
        if (!VK_CHECK(result)) {
            LOG_ERROR("Failed to create framebuffer!");
            return false;
        }
    }

    return true;
}

bool RenderingDestination::createDepthResources() {
    VkFormat depthFormat = VKUtils::findDepthFormat(_vkEngine->device->vkPhysicalDevice);

    auto depthTextureResult = createDepthTexture(depthFormat);
    if (depthTextureResult) {
        depthTexture = *depthTextureResult;
        return true;
    }
    return false;
}

std::optional<Texture*> RenderingDestination::createDepthTexture(VkFormat format) {
    auto imageResult = vax::createImage(
        _vkEngine,
        VkExtent3D{ _swapchainManager->swapchainExtent.width, _swapchainManager->swapchainExtent.height, 1 },
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (!imageResult) {
        return std::nullopt;
    }
    auto [depthImage, allocation] = imageResult.value();
    auto texture = new Texture(
        _vkEngine,
        "depth_texture",
        depthImage,
        allocation,
        vax::Size(_swapchainManager->swapchainExtent.width, _swapchainManager->swapchainExtent.height),
        format,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    vax::transitionImageLayout(
        _vkEngine,
        depthImage,
        format,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    texture->loadImageView();
    return std::make_optional(texture);
}

bool RenderingDestination::createRenderDesctinationTexture(VkExtent2D windowExtent) {
    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto imageResult = vax::createImage(
        _vkEngine,
        VkExtent3D{ windowExtent.width, windowExtent.height, 1 },
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL,
        drawImageUsages,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (imageResult) {
        auto [image, allocation] = imageResult.value();
        drawImage = std::make_unique<Texture>(
            Texture(
                _vkEngine,
                "render_destination",
                image,
                allocation,
                vax::Size(windowExtent.width, windowExtent.height),
                VK_FORMAT_R16G16B16A16_SFLOAT
            )
        );
        std::cout << "Created render destination texture!" << drawImage->name << std::endl;
        std::cout << "Sampler: " << (drawImage->sampler == nullptr ? "nullptr" : "not nullptr") << std::endl;
        drawImage->loadImageView();
        if (auto sampler = Sampler::createSampler(_vkEngine->device)) {
            drawImage->sampler = std::move(*sampler);
        }
        return true;
    }
    LOG_ERROR("Failed to create render destination texture!");
    return false;
}