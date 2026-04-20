#include "texture.h"
#include "TextureLoader.hpp"
#include "imageUtils.h"

using namespace vax;

void vax::textures::Texture::loadImageView() {
    textureImageView = vax::createImageView(
        _device.get().vkDevice, textureImage, format, aspectMask
    ).value();
}

void vax::textures::Texture::destroy() {
    _logger.info("Destroying texture...");
    if (textureImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device.get().vkDevice, textureImageView, nullptr);
        textureImageView = VK_NULL_HANDLE;
    }
    if (textureImage != VK_NULL_HANDLE) {
        vmaDestroyImage(_allocator, textureImage, _allocation);
        _allocation = VK_NULL_HANDLE;
        textureImage = VK_NULL_HANDLE;
    }
    name.clear();
    sampler = nullptr;
    size = math::SizeUI::zero();
    format = VK_FORMAT_UNDEFINED;
    aspectMask = VK_IMAGE_ASPECT_NONE;
}

bool vax::textures::Texture::isValid() const {
    return textureImage != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE;
}

std::optional<vax::textures::Texture*> vax::textures::Texture::makeCopy(VkCommandBuffer commandBuffer) const {
    auto other = new Texture(_device.get(), _allocator);
    if (copyTo(*other, commandBuffer)) {
        return std::make_optional(other);
    }
    Logger::getInstance().error("Failed to make copy of texture");
    delete other;
    return std::nullopt;
}

bool vax::textures::Texture::copyTo(vax::textures::Texture& other, VkCommandBuffer commandBuffer) const {
    if (!isValid()) {
        return false;
    }

    if (other.isValid()) {
        other.destroy();
    }

    VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

    blitRegion.srcOffsets[1].x = size.width;
    blitRegion.srcOffsets[1].y = size.height;
    blitRegion.srcOffsets[1].z = 1;

    blitRegion.dstOffsets[1].x = other.size.width;
    blitRegion.dstOffsets[1].y = other.size.height;
    blitRegion.dstOffsets[1].z = 1;

    blitRegion.srcSubresource.aspectMask = aspectMask;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;
    blitRegion.srcSubresource.mipLevel = 0;

    blitRegion.dstSubresource.aspectMask = aspectMask;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;
    blitRegion.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
    blitInfo.dstImage = other.textureImage;
    blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    blitInfo.srcImage = textureImage;
    blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blitInfo.filter = VK_FILTER_LINEAR;
    blitInfo.regionCount = 1;
    blitInfo.pRegions = &blitRegion;

    vkCmdBlitImage2(commandBuffer, &blitInfo);
    return true;
}