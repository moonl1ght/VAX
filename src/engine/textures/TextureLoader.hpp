#pragma once

#include "Buffer.hpp"
#include "luna.h"
#include "VKUtils.hpp"
#include "Texture.hpp"
#include "VKEngine.hpp"
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
    Texture* createRenderDesctinationTexture(VkExtent2D windowExtent);

private:
    void createImage(
        VkExtent3D extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
};