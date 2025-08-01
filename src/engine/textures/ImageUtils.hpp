#pragma once

#include "luna.h"

class VKEngine;

namespace vax {

    void transitionImageLayout(
        VKEngine* vkEngine,
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    );

    void copyBufferToImage(
        VKEngine* vkEngine,
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height
    );

    std::optional<VkImageView> createImageView(
        VkDevice device,
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectMask
    );

    std::optional<std::pair<VkImage, VmaAllocation>> createImage(
        VKEngine* vkEngine,
        VkExtent3D extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
};