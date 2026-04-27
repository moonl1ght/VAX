#include "textureLoader.h"
#include "imageUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./deps/stb_image.h"

using namespace vax::textures;
using namespace vax;

Texture* TextureLoader::loadTexture(std::string path, bool isAutoLoadImageView) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    auto stagingBuffer = vk::Buffer::allocateAndFillData(
        *vkEngine->device,
        pixels,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    ).value();

    stbi_image_free(pixels);

    auto [textureImage, allocation] = vax::createImage(
        vkEngine->allocator,
        VkExtent3D{ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 },
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    ).value();

    VkQueue graphicsQueue = vkEngine->queueManager->graphicsQueue;

    auto commandBuffer = vkEngine->commandManager->createSingleTimeCommandBuffer();
    vax::transitionImageLayout(
        commandBuffer,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        graphicsQueue
    );
    auto commandBuffer1 = vkEngine->commandManager->createSingleTimeCommandBuffer();
    vax::copyBufferToImage(
        commandBuffer1,
        stagingBuffer.getVkBuffer(),
        textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight),
        graphicsQueue
    );
    auto commandBuffer2 = vkEngine->commandManager->createSingleTimeCommandBuffer();
    vax::transitionImageLayout(
        commandBuffer2,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        graphicsQueue
    );
    auto texture = new Texture(
        *vkEngine->device,
        vkEngine->allocator,
        path,
        textureImage,
        allocation,
        math::SizeUI(texWidth, texHeight),
        VK_FORMAT_R8G8B8A8_SRGB
    );
    if (isAutoLoadImageView) {
        texture->loadImageView();
    }
    if (auto sampler = vax::textures::Sampler::createSampler(*vkEngine->device)) {
        texture->sampler = std::make_unique<vax::textures::Sampler>(std::move(*sampler));
    }
    return texture;
}