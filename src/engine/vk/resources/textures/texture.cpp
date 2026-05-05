#include "texture.h"
#include "textureLoader.h"
#include "imageUtils.h"

using namespace vax::textures;
using namespace vax;

void Texture::cleanup() {
    if (isDetached()) _destroy(false);
}

void Texture::_destroy(bool inDestructor) {
    if (inDestructor && !_isDetached) {
        _logger.error("Texture must be detached before destruction");
        return;
    }
    if (_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device.get().vkDevice, _imageView, nullptr);
        _imageView = VK_NULL_HANDLE;
    }
    if (_image != VK_NULL_HANDLE) {
        vmaDestroyImage(_allocator, _image, _allocation);
        _allocation = VK_NULL_HANDLE;
        _image = VK_NULL_HANDLE;
    }
    _name.clear();
    _sampler = nullptr;
    _size = math::SizeUI::zero();
    _format = VK_FORMAT_UNDEFINED;
    _aspectMask = VK_IMAGE_ASPECT_NONE;
    _isDetached = true;
    _id = NullTextureId;
}

void Texture::loadImageView() {
    _imageView = utils::createImageView(
        _device.get().vkDevice, _image, _format, _aspectMask
    ).value();
}

bool vax::textures::Texture::isValid() const {
    return _image != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE;
}

std::optional<vax::textures::Texture*> vax::textures::Texture::makeCopy(VkCommandBuffer commandBuffer) const {
    auto other = new Texture(_device.get(), _allocator);
    if (copyTo(*other, commandBuffer)) {
        return std::make_optional(other);
    }
    _logger.error("Failed to make copy of texture");
    delete other;
    return std::nullopt;
}

bool vax::textures::Texture::copyTo(vax::textures::Texture& other, VkCommandBuffer commandBuffer) const {
    if (!isValid()) {
        return false;
    }

    if (other.isValid()) {
        other._destroy(false);
    }

    VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

    blitRegion.srcOffsets[1].x = _size.width;
    blitRegion.srcOffsets[1].y = _size.height;
    blitRegion.srcOffsets[1].z = 1;

    blitRegion.dstOffsets[1].x = other._size.width;
    blitRegion.dstOffsets[1].y = other._size.height;
    blitRegion.dstOffsets[1].z = 1;

    blitRegion.srcSubresource.aspectMask = _aspectMask;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;
    blitRegion.srcSubresource.mipLevel = 0;

    blitRegion.dstSubresource.aspectMask = _aspectMask;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;
    blitRegion.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
    blitInfo.dstImage = other._image;
    blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    blitInfo.srcImage = _image;
    blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blitInfo.filter = VK_FILTER_LINEAR;
    blitInfo.regionCount = 1;
    blitInfo.pRegions = &blitRegion;

    vkCmdBlitImage2(commandBuffer, &blitInfo);
    return true;
}


std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfo() const {
    if (_sampler == nullptr || _imageView == VK_NULL_HANDLE) {
        _logger.error("Sampler or image view is not set");
        return std::nullopt;
    }
    return std::make_optional(VkDescriptorImageInfo{
        .sampler = _sampler->vkSampler,
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    });
}