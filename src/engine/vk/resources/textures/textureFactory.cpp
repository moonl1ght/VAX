#include "textureFactory.h"
#include "imageUtils.h"
#include "commandManager.h"

using namespace vax::textures;
using namespace vax;

std::optional<Texture> TextureFactory::makeDepthTextureDetached(
    VkFormat format,
    vax::math::SizeUI size
) {
    auto imageResult = utils::createImage(
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
    return std::make_optional(std::move(texture));
}

std::optional<TextureManager::TextureResource> TextureFactory::makeDepthTexture(
    VkFormat format,
    math::SizeUI size
) {
    auto texture = makeDepthTextureDetached(format, size);
    if (!texture) {
        return std::nullopt;
    }
    if (_textureManager) {
        auto attached = _textureManager->attach(std::move(*texture));
        if (attached) {
            return std::make_optional(std::move(*attached));
        }
        _logger.error("Failed to attach depth texture to manager");
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<Texture> TextureFactory::makeTextureDetached(
    VkFormat format,
    math::SizeUI size
) {
    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto imageResult = utils::createImage(
        _allocator,
        size.toExtent3D(),
        format,
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
            texture._sampler = std::make_unique<vax::textures::Sampler>(std::move(*sampler));
        }
        return std::make_optional(std::move(texture));
    }
    _logger.error("Failed to create render destination texture!");
    return std::nullopt;
}


std::optional<TextureManager::TextureResource> TextureFactory::makeTexture(
    VkFormat format,
    math::SizeUI size
) {
    auto texture = makeTextureDetached(format, size);
    if (!texture) {
        return std::nullopt;
    }
    if (_textureManager) {
        auto attached = _textureManager->attach(std::move(*texture));
        if (attached) {
            return std::make_optional(std::move(*attached));
        }
        _logger.error("Failed to attach texture to manager");
        return std::nullopt;
    }
    return std::nullopt;
}