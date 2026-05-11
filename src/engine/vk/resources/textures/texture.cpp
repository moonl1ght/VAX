#include "texture.h"
#include "textureLoader.h"
#include "imageUtils.h"

using namespace vax::textures;
using namespace vax;

void Texture::cleanup() {
    if (isDetached()) _destroy();
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
    _sampler = std::nullopt;
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

std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfo() const {
    if (!_sampler.has_value() || _imageView == VK_NULL_HANDLE) {
        _logger.error("Sampler or image view is not set");
        return std::nullopt;
    }
    return std::make_optional(VkDescriptorImageInfo{
        .sampler = _sampler->vkSampler,
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    });
}

std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfoNoSampler() const {
    if (_imageView == VK_NULL_HANDLE) {
        _logger.error("Image view is not set");
        return std::nullopt;
    }
    return std::make_optional(VkDescriptorImageInfo{
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    });
}

std::optional<VkDescriptorImageInfo> Texture::descriptorImageInfo(const Sampler& sampler) const {
    if (_imageView == VK_NULL_HANDLE) {
        _logger.error("Image view is not set");
        return std::nullopt;
    }
    return std::make_optional(VkDescriptorImageInfo{
        .sampler = sampler.vkSampler,
        .imageView = _imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    });
}

void Texture::createSampler() {
    if (_sampler.has_value()) {
        return;
    }
    if (auto sampler = vax::textures::Sampler::createSampler(_device.get(), _name + "_texture_sampler")) {
        _sampler = std::make_optional(std::move(*sampler));
    }
}