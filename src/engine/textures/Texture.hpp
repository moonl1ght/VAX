#ifndef Texture_hpp
#define Texture_hpp

#include "luna.h"
#include "VKStack.hpp"
#include <iostream>
#include "VAXMath.hpp"
#include "VKUtils.hpp"
#include "Sampler.hpp"

class Texture final {
public:
    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory vkBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize dSize = 0;
    VkImageView textureImageView = VK_NULL_HANDLE;
    vax::Size size = vax::Size::zero();
    std::unique_ptr<Sampler> sampler = nullptr;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;

    Texture(
        VkDevice device,
        VkImage textureImage,
        VkDeviceMemory vkBufferMemory,
        VkDeviceSize dSize,
        vax::Size size,
        VkFormat format,
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    )
        : _device(device)
        , textureImage(textureImage)
        , vkBufferMemory(vkBufferMemory)
        , dSize(dSize)
        , size(size)
        , format(format)
        , aspectMask(aspectMask) {
    }

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept {
        _device = other._device;
        std::swap(textureImage, other.textureImage);
        std::swap(vkBufferMemory, other.vkBufferMemory);
        std::swap(dSize, other.dSize);
        std::swap(size, other.size);
        std::swap(textureImageView, other.textureImageView);
        std::swap(format, other.format);
        std::swap(aspectMask, other.aspectMask);
        other._device = VK_NULL_HANDLE;
    }
    ~Texture() {
        if (_device != VK_NULL_HANDLE) {
            if (textureImage != VK_NULL_HANDLE) {
                vkDestroyImage(_device, textureImage, nullptr);
                textureImage = VK_NULL_HANDLE;
            }
            if (vkBufferMemory != VK_NULL_HANDLE) {
                vkFreeMemory(_device, vkBufferMemory, nullptr);
                vkBufferMemory = VK_NULL_HANDLE;
            }
            if (textureImageView != VK_NULL_HANDLE) {
                vkDestroyImageView(_device, textureImageView, nullptr);
                textureImageView = VK_NULL_HANDLE;
            }
            if (format != VK_FORMAT_UNDEFINED) {
                format = VK_FORMAT_UNDEFINED;
            }
            if (aspectMask != VK_IMAGE_ASPECT_NONE) {
                aspectMask = VK_IMAGE_ASPECT_NONE;
            }
        }
    }

    Texture& operator=(Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            if (_device != VK_NULL_HANDLE) {
                if (textureImage != VK_NULL_HANDLE) {
                    vkDestroyImage(_device, textureImage, nullptr);
                    textureImage = VK_NULL_HANDLE;
                }
                if (vkBufferMemory != VK_NULL_HANDLE) {
                    vkFreeMemory(_device, vkBufferMemory, nullptr);
                    vkBufferMemory = VK_NULL_HANDLE;
                }
                if (textureImageView != VK_NULL_HANDLE) {
                    vkDestroyImageView(_device, textureImageView, nullptr);
                    textureImageView = VK_NULL_HANDLE;
                }
            }

            // Move resources from other
            _device = other._device;
            dSize = other.dSize;
            size = other.size;
            textureImage = other.textureImage;
            vkBufferMemory = other.vkBufferMemory;
            textureImageView = other.textureImageView;
            format = other.format;
            aspectMask = other.aspectMask;

            // Reset other
            other._device = VK_NULL_HANDLE;
            other.textureImage = VK_NULL_HANDLE;
            other.vkBufferMemory = VK_NULL_HANDLE;
            other.textureImageView = VK_NULL_HANDLE;
            other.dSize = 0;
            other.size = vax::Size::zero();
            other.format = VK_FORMAT_UNDEFINED;
            other.aspectMask = VK_IMAGE_ASPECT_NONE;
        }
        return *this;
    }

    void loadImageView();

private:
    VkDevice _device;
};

#endif