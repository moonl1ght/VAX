#pragma once

#include "luna.h"
#include "texture.h"
#include "device.h"
#include "vkEngine.h"

namespace vax::textures {
    class TextureBuilder {
    public:
        explicit TextureBuilder(
            const vax::vk::Device& device,
            VmaAllocator allocator
        ) : _device(device), _allocator(allocator) {
        };

        std::optional<vax::textures::Texture> buildDepthTexture(VkFormat format, vax::math::SizeUI size, vax::vk::Engine* vkEngine);
        std::optional<vax::textures::Texture> buildTexture(VkFormat format, vax::math::SizeUI size, vax::vk::Engine* vkEngine);

    private:
        Logger _logger = Logger("TextureBuilder");
        std::reference_wrapper<const vax::vk::Device> _device;
        VmaAllocator _allocator;
    };
}