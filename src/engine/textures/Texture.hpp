#ifndef Texture_hpp
#define Texture_hpp

#include "luna.h"
#include "VKStack.hpp"
#include <iostream>
#include "VAXMath.hpp"
#include "VKUtils.hpp"

class Texture final {
public:
    VkImage textureImage;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize dSize;
    VkImageView textureImageView = VK_NULL_HANDLE;
    vax::Size size = vax::Size::zero();

    Texture(
        VkDevice device,
        VkImage textureImage,
        VkDeviceSize dSize,
        vax::Size size
    ) : device(device), textureImage(textureImage), dSize(dSize), size(size) {
    };

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) {
        if (this != &other) {
            std::swap(textureImage, other.textureImage);
            std::swap(vkBufferMemory, other.vkBufferMemory);
            std::swap(dSize, other.dSize);
            std::swap(size, other.size);
            std::swap(textureImageView, other.textureImageView);
        }
    }
    ~Texture() {
        vkDestroyImage(device, textureImage, nullptr);
        vkFreeMemory(device, vkBufferMemory, nullptr);
        if (textureImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(device, textureImageView, nullptr);
        }
    }

    Texture& operator=(Texture& other) = delete;
    Texture& operator=(Texture&& other) {
        if (this != &other) {
            std::swap(textureImage, other.textureImage);
            std::swap(vkBufferMemory, other.vkBufferMemory);
            std::swap(dSize, other.dSize);
            std::swap(size, other.size);
            std::swap(textureImageView, other.textureImageView);
        }
        return *this;
    }

    void loadImageView();

private:
    VkDevice device;
};

#endif