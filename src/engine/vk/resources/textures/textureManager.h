#pragma once

#include "luna.h"
#include "device.h"
#include "texture.h"
#include "resourceHandle.h"
#include <unordered_map>

namespace vax::textures {
    class TextureFactory;
}

namespace vax::vk {
    class DescriptorSetWriter;
}

namespace vax {
    class TextureManager final {
    public:
        using TextureResource = std::pair<TextureHandle, textures::Texture*>;
        using SamplerResource = std::pair<SamplerHandle, vax::textures::Sampler*>;

        explicit TextureManager(
            const vk::Device& device,
            VmaAllocator allocator
        )
            : _device(device)
            , _allocator(allocator) {
        };

        ~TextureManager() {
            fullCleanup();
        }

        TextureManager(const TextureManager& other) = delete;
        TextureManager(TextureManager&& other) noexcept = delete;
        TextureManager& operator=(const TextureManager& other) = delete;
        TextureManager& operator=(TextureManager&& other) noexcept = delete;

        void fullCleanup();

        vax::textures::TextureFactory createTextureFactory() const;

        std::optional<TextureResource> find(TextureHandle handle);

        bool deleteTexture(TextureHandle handle);

        std::optional<textures::Texture> detach(TextureHandle handle);

        std::optional<TextureResource> attach(textures::Texture&& texture);

        std::optional<SamplerResource> findSampler(SamplerHandle handle);

        bool deleteSampler(SamplerHandle handle);

        std::optional<SamplerResource> insertSampler(vax::textures::Sampler&& sampler);

        std::optional<SamplerResource> getPBRSampler();

        void updateDescriptorWriterWithAllTextures(
            vax::vk::DescriptorSetWriter& descriptorWriter,
            uint32_t binding,
            bool useSampler
        ) const;

    private:
        utils::Logger _logger = utils::Logger("TextureManager");

        std::reference_wrapper<const vk::Device> _device;
        VmaAllocator _allocator;
        // TODO: change to vector + use generation for stability
        // maybe vector of vectors of buffers?
        std::unordered_map<TextureId, textures::Texture> _pool;
        std::unordered_map<SamplerId, vax::textures::Sampler> _samplerPool;
        TextureId _lastId = 0;
        SamplerId _lastSamplerId = PBRSamplerId;
    };
}