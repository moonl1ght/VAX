#include "textureLoader.h"
#include "imageUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./deps/stb_image.h"
#include "buffer.h"
#include "textureFactory.h"
#include "textureTaskScheduler.h"

using namespace vax::textures;
using namespace vax;

std::optional<TextureManager::TextureResource> TextureLoader::loadTexture(
    std::string name,
    std::span<unsigned char> data,
    VkQueue submitQueue
) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load_from_memory(
        data.data(), data.size(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha
    );
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels) {
        _logger.error("Failed to load pixels");
        return std::nullopt;
    }
    return _loadTexture(name, pixels, submitQueue, texWidth, texHeight, texChannels);
}

std::optional<TextureManager::TextureResource> TextureLoader::loadTexture(
    std::string path, VkQueue submitQueue
) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        _logger.error("Failed to load pixels");
        return std::nullopt;
    }
    return _loadTexture(path, pixels, submitQueue, texWidth, texHeight, texChannels);
}

std::optional<TextureManager::TextureResource> TextureLoader::_loadTexture(
    std::string name,
    unsigned char* pixels,
    VkQueue submitQueue,
    int texWidth,
    int texHeight,
    int texChannels
) {
    VkDeviceSize imageSize = texWidth * texHeight * texChannels;
    auto stagingBuffer = vk::Buffer::allocateAndFillData(
        _device.get(),
        pixels,
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stbi_image_free(pixels);

    if (!stagingBuffer.has_value()) {
        return std::nullopt;
    }

    auto textureFactory = _textureManager.get().createTextureFactory();

    auto texture = textureFactory.makeTexture(
        name,
        VK_FORMAT_R8G8B8A8_SRGB,
        math::SizeUI(texWidth, texHeight),
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
    );

    if (!texture.has_value()) {
        return std::nullopt;
    }

    auto textureTaskScheduler = textures::TextureTaskScheduler(_device.get(), _commandManager.get());

    auto commandBuffer = _commandManager.get().createSingleTimeCommandBuffer();
    auto taskSchedulerInline = TextureTaskSchedulerInline(_device.get(), commandBuffer);
    commandBuffer.begin();
    taskSchedulerInline.transitionTextureLayout(
        *(texture->second),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    taskSchedulerInline.copyBufferToTexture(*stagingBuffer, *(texture->second));
    taskSchedulerInline.transitionTextureLayout(
        *(texture->second),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);

    stagingBuffer->cleanup();
    return texture;
}