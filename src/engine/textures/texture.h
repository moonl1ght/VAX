#pragma once

#include "luna.h"
#include "VAXMath.hpp"
#include "sampler.h"

namespace vax::textures {
    class Texture final {
    public:
        std::string name;
        VkImage textureImage = VK_NULL_HANDLE;
        VkImageView textureImageView = VK_NULL_HANDLE;
        vax::math::SizeUI size = vax::math::SizeUI::zero();
        std::unique_ptr<vax::textures::Sampler> sampler = nullptr;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;

        Texture(const vax::vk::Device& device, VmaAllocator allocator)
            : _device(device), _allocator(allocator) {
        };

        Texture(
            const vax::vk::Device& device,
            VmaAllocator allocator,
            std::string name,
            VkImage textureImage,
            VmaAllocation allocation,
            vax::math::SizeUI size,
            VkFormat format,
            VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
        )
            : _device(device)
            , _allocator(allocator)
            , name(name)
            , textureImage(textureImage)
            , _allocation(allocation)
            , size(size)
            , format(format)
            , aspectMask(aspectMask) {
        }

        Texture(const Texture& other) = delete;

        Texture(Texture&& other) noexcept
            : _device(other._device)
            , _allocator(other._allocator)
            , sampler(std::move(other.sampler))
            , name(other.name)
            , textureImage(other.textureImage)
            , _allocation(other._allocation)
            , size(other.size)
            , textureImageView(other.textureImageView)
            , format(other.format)
            , aspectMask(other.aspectMask)
        {
            other.name.clear();
            other.sampler = nullptr;
            other.textureImage = VK_NULL_HANDLE;
            other._allocation = VK_NULL_HANDLE;
            other.textureImageView = VK_NULL_HANDLE;
            other.size = vax::math::SizeUI::zero();
            other.format = VK_FORMAT_UNDEFINED;
            other.aspectMask = VK_IMAGE_ASPECT_NONE;
        }

        ~Texture() {
            destroy();
        }

        Texture& operator=(Texture& other) = delete;

        Texture& operator=(Texture&& other) noexcept {
            if (this != &other) {
                destroy();
                sampler = std::move(other.sampler);
                _device = other._device;
                _allocator = other._allocator;
                name = other.name;
                size = other.size;
                textureImage = other.textureImage;
                _allocation = other._allocation;
                textureImageView = other.textureImageView;
                format = other.format;
                aspectMask = other.aspectMask;

                other.name.clear();
                other.textureImage = VK_NULL_HANDLE;
                other._allocation = VK_NULL_HANDLE;
                other.textureImageView = VK_NULL_HANDLE;
                other.sampler = nullptr;
                other.size = vax::math::SizeUI::zero();
                other.format = VK_FORMAT_UNDEFINED;
                other.aspectMask = VK_IMAGE_ASPECT_NONE;
            }
            return *this;
        }

        bool isValid() const;
        void loadImageView();
        void destroy();

        std::optional<Texture*> makeCopy(VkCommandBuffer commandBuffer) const;
        bool copyTo(Texture& other, VkCommandBuffer commandBuffer) const;

    private:
        VmaAllocation _allocation = VK_NULL_HANDLE;
        VmaAllocator _allocator = VK_NULL_HANDLE;
        std::reference_wrapper<const vax::vk::Device> _device;
    };
}