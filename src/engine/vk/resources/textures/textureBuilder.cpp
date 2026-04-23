#include "textureBuilder.h"
#include "imageUtils.h"

using namespace vax::textures;

std::optional<Texture> vax::textures::TextureBuilder::buildDepthTexture(VkFormat format, vax::math::SizeUI size, vax::vk::Engine* vkEngine) {
    _logger.debug("Building depth texture...");
    auto imageResult = vax::createImage(
        _allocator,
        size.toExtent3D(),
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (!imageResult) {
        _logger.error("Failed to create depth texture!");
        return std::nullopt;
    }
    auto [depthImage, allocation] = imageResult.value();
    auto texture = vax::textures::Texture(
        _device.get(),
        _allocator,
        "depth_texture",
        depthImage,
        allocation,
        size,
        format,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    auto commandBuffer = vkEngine->commandManager->createSingleTimeCommandBuffer();
    vax::transitionImageLayout(
        commandBuffer,
        depthImage,
        format,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        vkEngine->queueManager->graphicsQueue,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    texture.loadImageView();
    return std::make_optional(std::move(texture));
}

std::optional<Texture> vax::textures::TextureBuilder::buildTexture(VkFormat format, vax::math::SizeUI size, vax::vk::Engine* vkEngine) {
    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto imageResult = vax::createImage(
        _allocator,
        size.toExtent3D(),
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL,
        drawImageUsages,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (imageResult) {
        auto [image, allocation] = imageResult.value();
        auto texture = vax::textures::Texture(
            _device.get(),
                _allocator,
                "render_destination",
                image,
                allocation,
                size,
                VK_FORMAT_R16G16B16A16_SFLOAT
        );
        texture.loadImageView();
        if (auto sampler = vax::textures::Sampler::createSampler(_device.get())) {
            texture.sampler = std::make_unique<vax::textures::Sampler>(std::move(*sampler));
        }
        return std::make_optional(std::move(texture));
    }
    _logger.error("Failed to create render destination texture!");
    return std::nullopt;
}