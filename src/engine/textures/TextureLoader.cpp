#include "TextureLoader.hpp"
#include "ImageUtils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "./deps/stb_image.h"

Texture* TextureLoader::loadTexture(std::string path, bool isAutoLoadImageView) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    Buffer stagingBuffer = Buffer(
        vkEngine,
        pixels,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stbi_image_free(pixels);

    auto [textureImage, allocation] = vax::createImage(
        vkEngine,
        VkExtent3D{ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 },
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ).value();

    vax::transitionImageLayout(
        vkEngine,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
    vax::copyBufferToImage(
        vkEngine,
        stagingBuffer.vkBuffer,
        textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );
    vax::transitionImageLayout(
        vkEngine,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    auto texture = new Texture(
        vkEngine,
        path,
        textureImage,
        allocation,
        vax::Size(texWidth, texHeight),
        VK_FORMAT_R8G8B8A8_SRGB
    );
    if (isAutoLoadImageView) {
        texture->loadImageView();
    }
    if (auto sampler = Sampler::createSampler(vkEngine->device)) {
        texture->sampler = std::move(*sampler);
    }
    return texture;
}