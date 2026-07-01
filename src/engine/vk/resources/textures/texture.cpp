#include "texture.h"
#include "imageUtils.h"
#include "textureLoader.h"

using namespace vax::vk;
using namespace vax;

void Texture::cleanup() {
    if (isDetached())
        _destroy();
}

void Texture::_destroy() {
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
    _size = math::SizeUI::zero();
    _format = VK_FORMAT_UNDEFINED;
    _aspectMask = VK_IMAGE_ASPECT_NONE;
    _isDetached = true;
    _id = NullTextureId;
}

void Texture::loadImageView(VkImageViewType viewType, uint32_t layerCount, uint32_t levelCount) {
    _imageView =
        createImageView(_device.get().vkDevice, _image, _format, _aspectMask, viewType, layerCount, levelCount).value();
}

bool vax::vk::Texture::isValid() const { return _image != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE; }

std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfoNoSampler() const {
    if (_imageView == VK_NULL_HANDLE) {
        _logger.error("Image view is not set");
        return std::nullopt;
    }
    return std::make_optional(
        VkDescriptorImageInfo{
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        }
    );
}

std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfo(const Sampler& sampler) const {
    if (_imageView == VK_NULL_HANDLE) {
        _logger.error("Image view is not set");
        return std::nullopt;
    }
    return std::make_optional(
        VkDescriptorImageInfo{
        .sampler = sampler.vkSampler,
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        }
    );
}