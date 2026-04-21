#pragma once

#include "luna.h"

namespace vax::vk {
    class CommandBuffer;
    class Engine;
};

namespace vax {

    void transitionImageLayout(
        vax::vk::CommandBuffer& commandBuffer,
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkQueue submitQueue,
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    );

    void copyImageToImage(
        VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize
    );

    void transitionImage(
        VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout
    );

    void copyBufferToImage(
        vax::vk::CommandBuffer& commandBuffer,
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        VkQueue submitQueue
    );

    std::optional<VkImageView> createImageView(
        VkDevice device,
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectMask
    );

    std::optional<std::pair<VkImage, VmaAllocation>> createImage(
        VmaAllocator allocator,
        VkExtent3D extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags aspectMask);
};