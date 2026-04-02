#pragma once

#include "luna.h"

namespace vax { class VkEngine; }

namespace vax {

    void transitionImageLayout(
        vax::VkEngine* vkEngine,
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    );

    void copyImageToImage(
        VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize
    );

    void transitionImage(
        VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout
    );

    void copyBufferToImage(
        vax::VkEngine* vkEngine,
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
        vax::VkEngine* vkEngine,
        VkExtent3D extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectMask);
};