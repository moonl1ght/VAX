#ifndef Texture_hpp
#define Texture_hpp

#include "luna.h"
#include "VAXMath.hpp"
#include "Sampler.hpp"
#include "VKObject.hpp"

class Texture final : public VKObject {
public:
    std::string name;
    VkImage textureImage = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    vax::Size size = vax::Size::zero();
    std::unique_ptr<Sampler> sampler = nullptr;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;

    Texture() = default;

    Texture(
        VKEngine* vkEngine,
        std::string name,
        VkImage textureImage,
        VmaAllocation allocation,
        vax::Size size,
        VkFormat format,
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    )
        : VKObject(vkEngine)
        , name(name)
        , textureImage(textureImage)
        , allocation(allocation)
        , size(size)
        , format(format)
        , aspectMask(aspectMask) {
        Logger::getInstance().log("Creating texture! Name: " + name);
    }

    Texture(const Texture& other) = delete;

    Texture(Texture&& other) noexcept {
        Logger::getInstance().log("Moving texture! sd");
        std::swap(name, other.name);
        std::swap(textureImage, other.textureImage);
        std::swap(allocation, other.allocation);
        std::swap(size, other.size);
        std::swap(textureImageView, other.textureImageView);
        std::swap(format, other.format);
        std::swap(aspectMask, other.aspectMask);
        std::cout << "Swapped sampler! This: " << (sampler == nullptr ? "nullptr" : "not nullptr") << " Other: " << (other.sampler == nullptr ? "nullptr" : "not nullptr") << std::endl;
        std::swap(sampler, other.sampler);
        std::swap(vkEngine, other.vkEngine);
    }

    ~Texture() {
        destroy();
    }

    Texture& operator=(Texture& other) = delete;

    Texture& operator=(Texture&& other) noexcept {
        Logger::getInstance().log("Moving texture!");
        if (this != &other) {
            sampler = std::move(other.sampler);
            vkEngine = other.vkEngine;
            name = other.name;
            size = other.size;
            textureImage = other.textureImage;
            allocation = other.allocation;
            textureImageView = other.textureImageView;
            format = other.format;
            aspectMask = other.aspectMask;

            other.vkEngine = VK_NULL_HANDLE;
            other.name.clear();
            other.textureImage = VK_NULL_HANDLE;
            other.allocation = VK_NULL_HANDLE;
            other.textureImageView = VK_NULL_HANDLE;
            other.sampler = nullptr;
            other.size = vax::Size::zero();
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
};

#endif