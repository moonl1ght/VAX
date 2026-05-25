#include "textureFactory.h"
#include "commandManager.h"
#include "imageUtils.h"

using namespace vax::textures;
using namespace vax;

std::optional<Texture> TextureFactory::makeDepthTextureDetached(VkFormat format, vax::math::SizeUI size) {
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
    VkDebugUtilsObjectNameInfoEXT nameInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = VK_OBJECT_TYPE_IMAGE,
        .objectHandle = reinterpret_cast<size_t>(imageResult->first),
        .pObjectName = "depth_texture",
    };
    vax::vk::utils::pfnSetDebugUtilsObjectNameEXT(_device.get().vkDevice, &nameInfo);
    auto [depthImage, allocation] = imageResult.value();
    auto texture = vax::textures::Texture(
        _device.get(), _allocator, "depth_texture", depthImage, allocation, size, format, VK_IMAGE_ASPECT_DEPTH_BIT
    );
    return std::make_optional(std::move(texture));
}

std::optional<TextureManager::TextureResource> TextureFactory::makeDepthTexture(VkFormat format, math::SizeUI size) {
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
    std::string name, VkFormat format, math::SizeUI size, VkImageUsageFlags imageUsageFlags
) {
    auto imageResult = utils::createImage(
        _allocator,
        size.toExtent3D(),
        format,
        VK_IMAGE_TILING_OPTIMAL,
        imageUsageFlags,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (imageResult) {
        auto [image, allocation] = imageResult.value();
        auto texture = vax::textures::Texture(
            _device.get(), _allocator, name, image, allocation, size, format, VK_IMAGE_ASPECT_COLOR_BIT
        );
        if (!name.empty()) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VK_OBJECT_TYPE_IMAGE,
                .objectHandle = reinterpret_cast<size_t>(image),
                .pObjectName = name.c_str(),
            };
            vax::vk::utils::pfnSetDebugUtilsObjectNameEXT(_device.get().vkDevice, &nameInfo);
        }
        texture.loadImageView();
        return std::make_optional(std::move(texture));
    }
    _logger.error("Failed to create render destination texture!");
    return std::nullopt;
}

std::optional<TextureManager::TextureResource>
TextureFactory::makeTexture(std::string name, VkFormat format, math::SizeUI size, VkImageUsageFlags imageUsageFlags) {
    auto texture = makeTextureDetached(name, format, size, imageUsageFlags);
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