#pragma once

#include "device.h"
#include "luna.h"
#include "resourceHandle.h"
#include "texture.h"
#include <unordered_map>

namespace vax::textures {
class TextureFactory;
}

namespace vax::vk {
class DescriptorSetHandler;
}

namespace vax {
class TextureManager final {
  public:
    using TextureResource = std::pair<TextureHandle, textures::Texture*>;
    using SamplerResource = std::pair<SamplerHandle, vax::textures::Sampler*>;

    explicit TextureManager(const vk::Device& device, VmaAllocator allocator)
        : _device(device)
        , _allocator(allocator) {
        _globalSamplers.reserve(vax::MAX_GLOBAL_SAMPLERS);
    };

    ~TextureManager() { fullCleanup(); }

    TextureManager(const TextureManager& other) = delete;
    TextureManager(TextureManager&& other) noexcept = delete;
    TextureManager& operator=(const TextureManager& other) = delete;
    TextureManager& operator=(TextureManager&& other) noexcept = delete;

    void fullCleanup();

    bool setup();

    vax::textures::TextureFactory createTextureFactory() const;

    std::optional<TextureResource> find(TextureHandle handle);

    bool deleteTexture(TextureHandle handle);

    std::optional<textures::Texture> detach(TextureHandle handle);

    std::optional<TextureResource> attach(textures::Texture&& texture);

    std::optional<SamplerResource> getGlobalSampler(vax::GlobalSampler sampler);

    void updateDescriptorHandlerWithAllTextures(
        vax::vk::DescriptorSetHandler& descriptorHandler, uint32_t binding
    ) const;

  private:
    vax::Logger _logger = vax::Logger("TextureManager");

    std::reference_wrapper<const vk::Device> _device;
    VmaAllocator _allocator;
    // TODO: change to vector + use generation for stability
    // maybe vector of vectors of buffers?
    std::unordered_map<TextureId, textures::Texture> _pool;
    std::vector<vax::textures::Sampler> _globalSamplers;
    TextureId _lastId = 0;
};
} // namespace vax