#pragma once

#include "luna.h"
#include "texture.h"
#include "device.h"

namespace vax {
    class TextureManager;
}

namespace vax::vk {
    class CommandBuffer;
    class CommandManager;
}

namespace vax::textures {
    class TextureFactory final {
    public:
        explicit TextureFactory(
            const vax::vk::Device& device,
            VmaAllocator allocator,
            TextureManager* textureManager
        )
            : _device(device)
            , _allocator(allocator)
            , _textureManager(textureManager) {
        };

        explicit TextureFactory(
            const vax::vk::Device& device,
            VmaAllocator allocator
        )
            : _device(device)
            , _allocator(allocator)
            , _textureManager(nullptr) {
        };

        TextureFactory(const TextureFactory& other) = delete;
        TextureFactory& operator=(const TextureFactory& other) = delete;
        TextureFactory(TextureFactory&& other) noexcept = delete;
        TextureFactory& operator=(TextureFactory&& other) noexcept = delete;

        std::optional<Texture> makeDepthTexture(
            VkFormat format, math::SizeUI size
        );

        std::optional<Texture> makeTexture(
            VkFormat format, math::SizeUI size
        );

    private:
        utils::Logger _logger = utils::Logger("TextureFactory");
        std::reference_wrapper<const vk::Device> _device;
        TextureManager* _textureManager;
        VmaAllocator _allocator;
    };
}