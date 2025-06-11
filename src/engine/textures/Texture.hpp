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
    VkImage textureImage;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize dSize;
    VkImageView textureImageView = VK_NULL_HANDLE;
    vax::Size size = vax::Size::zero();
    std::unique_ptr<Sampler> sampler;

    Texture(
        VkDevice device,
        VkImage textureImage,
        VkDeviceSize dSize,
        vax::Size size
    ) : _device(device), textureImage(textureImage), dSize(dSize), size(size) { }

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) {
        std::swap(textureImage, other.textureImage);
        std::swap(vkBufferMemory, other.vkBufferMemory);
        std::swap(dSize, other.dSize);
        std::swap(size, other.size);
        std::swap(textureImageView, other.textureImageView);
        std::swap(_device, other._device);
    }
    ~Texture() {
        vkDestroyImage(_device, textureImage, nullptr);
        vkFreeMemory(_device, vkBufferMemory, nullptr);
        if (textureImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(_device, textureImageView, nullptr);
        }
    }

    Texture& operator=(Texture& other) = delete;
    Texture& operator=(Texture&& other) {
        if (this != &other) {
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
            _device = other._device;
            dSize = other.dSize;
            size = other.size;
            std::swap(textureImage, other.textureImage);
            std::swap(vkBufferMemory, other.vkBufferMemory);
            std::swap(textureImageView, other.textureImageView); 
            other._device = VK_NULL_HANDLE;
            other.dSize = 0;
            other.size = vax::Size::zero();
        }
        return *this;
    }

    void loadImageView();

private:
    VkDevice _device;
};

#endif