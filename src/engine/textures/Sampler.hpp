#ifndef Sampler_hpp
#define Sampler_hpp

#include <iostream>

#include "device.h"
#include "luna.h"

class Sampler final {
public:
    VkSampler vkSampler;

    static std::optional<std::unique_ptr<Sampler>> createSampler(vax::Device* device) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device->vkPhysicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        VkSampler vkSampler;
        if (vkCreateSampler(device->vkDevice, &samplerInfo, nullptr, &vkSampler) != VK_SUCCESS) {
            Logger::getInstance().error("failed to create texture sampler!");
            return std::nullopt;
        }
        return std::make_unique<Sampler>(vkSampler, device);
    }

    Sampler(VkSampler vkSampler, vax::Device* device) : vkSampler(vkSampler), _device(device) {}
    Sampler(const Sampler& other) = delete;
    Sampler(Sampler&& other) noexcept {
        std::swap(vkSampler, other.vkSampler);
        std::swap(_device, other._device);
    }

    Sampler& operator=(const Sampler& other) = delete;
    Sampler& operator=(Sampler&& other) noexcept {
        if (this != &other) {
            if (vkSampler != VK_NULL_HANDLE) {
                vkDestroySampler(_device->vkDevice, vkSampler, nullptr);
                vkSampler = VK_NULL_HANDLE;
            }
            _device = other._device;
            std::swap(vkSampler, other.vkSampler);
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~Sampler() { vkDestroySampler(_device->vkDevice, vkSampler, nullptr); }

private:
    vax::Device* _device;
};

#endif