#pragma once

#include "buffer.h"
#include "commandBuffer.h"
#include "commandManager.h"
#include "luna.h"
#include "texture.h"
#include "textureManager.h"
#include "vkUtils.h"
#include <span>

namespace vax::textures {
class TextureLoader final {
  public:
    TextureLoader(
        const vax::vk::Device& device, vax::TextureManager& textureManager, vax::vk::CommandManager& commandManager
    )
        : _device(device)
        , _textureManager(textureManager)
        , _commandManager(commandManager) {};

    TextureLoader(const TextureLoader& other) = delete;
    TextureLoader& operator=(const TextureLoader& other) = delete;
    TextureLoader(TextureLoader&& other) noexcept = delete;
    TextureLoader& operator=(TextureLoader&& other) noexcept = delete;

    std::optional<TextureManager::TextureResource> loadTexture(std::string path, VkQueue submitQueue = nullptr);
    std::optional<TextureManager::TextureResource>
    loadTexture(std::string name, std::span<unsigned char> data, VkQueue submitQueue = nullptr);

    void loadStaged(vax::vk::CommandBuffer& commandBuffer);

    void cleanupStaged();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("TextureLoader");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::reference_wrapper<vax::TextureManager> _textureManager;
    std::reference_wrapper<vax::vk::CommandManager> _commandManager;

    std::optional<TextureManager::TextureResource> _loadTexture(
        std::string name, unsigned char* pixels, VkQueue submitQueue, int texWidth, int texHeight, int texChannels
    );

    std::optional<TextureManager::TextureResource> _loadKTXTexture(std::string path, VkQueue submitQueue);

    std::vector<std::pair<vax::vk::Buffer, vax::TextureManager::TextureResource>> _stagingTextures;
};
} // namespace vax::textures