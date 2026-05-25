#pragma once

#include "device.h"
#include "luna.h"
#include "texture.h"
#include "textureManager.h"

namespace vax::vk {
class CommandBuffer;
class CommandManager;
} // namespace vax::vk

namespace vax::textures {
class TextureFactory final {
  public:
    explicit TextureFactory(const vax::vk::Device& device, VmaAllocator allocator, TextureManager* textureManager)
        : _device(device)
        , _allocator(allocator)
        , _textureManager(textureManager) {};

    explicit TextureFactory(const vax::vk::Device& device, VmaAllocator allocator)
        : _device(device)
        , _allocator(allocator)
        , _textureManager(nullptr) {};

    TextureFactory(const TextureFactory& other) = delete;
    TextureFactory& operator=(const TextureFactory& other) = delete;
    TextureFactory(TextureFactory&& other) noexcept = delete;
    TextureFactory& operator=(TextureFactory&& other) noexcept = delete;

    std::optional<vax::TextureManager::TextureResource> makeDepthTexture(VkFormat format, math::SizeUI size);

    std::optional<Texture> makeDepthTextureDetached(VkFormat format, math::SizeUI size);

    std::optional<vax::TextureManager::TextureResource>
    makeTexture(std::string name, VkFormat format, math::SizeUI size, VkImageUsageFlags imageUsageFlags);

    std::optional<Texture>
    makeTextureDetached(std::string name, VkFormat format, math::SizeUI size, VkImageUsageFlags imageUsageFlags);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("TextureFactory");
    std::reference_wrapper<const vk::Device> _device;
    TextureManager* _textureManager;
    VmaAllocator _allocator;
};
} // namespace vax::textures