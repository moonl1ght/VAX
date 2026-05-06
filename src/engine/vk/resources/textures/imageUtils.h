#pragma once

#include "luna.h"

namespace vax::vk {
    class CommandBuffer;
    class Engine;
};

namespace vax::textures::utils {

    // TODO: Move to textureTaskScheduler
    void copyImageToImage(
        VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize
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
        VkMemoryPropertyFlags properties,
        uint32_t layers = 1
    );
};