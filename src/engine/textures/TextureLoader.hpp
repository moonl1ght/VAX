#pragma once

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"
#include "VKObjectBuilder.hpp"

class TextureLoader final : public VKObjectBuilder {
public:
    static std::optional<VkImageView> createImageView(
        VkDevice device,
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectMask
    );

    Texture* loadTexture(std::string path, bool isAutoLoadImageView = true);
    Texture* createDepthTexture(VkFormat format);
    std::optional<Texture*> createRenderDesctinationTexture(VkExtent2D windowExtent);

private:
    std::optional<std::pair<VkImage, VmaAllocation>> createImage(
        VkExtent3D extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
};