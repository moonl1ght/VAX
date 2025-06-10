#ifndef Sampler_hpp
#define Sampler_hpp

#include <iostream>

#include "Device.hpp"
#include "luna.h"

class Sampler final {
public:
    VkSampler vkSampler;

    static std::optional<Sampler> createSampler(vax::Device* device) {
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
        if (vkCreateSampler(device->vkDevice, &samplerInfo, nullptr, &vkSampler) !=
            VK_SUCCESS) {
            std::cerr << "failed to create texture sampler!" << std::endl;
            return std::nullopt;
        }
        return Sampler(vkSampler);
    }

    Sampler(VkSampler vkSampler) : vkSampler(vkSampler) {}
    Sampler(const Sampler& other) = delete;
    Sampler(Sampler&& other) {
        if (this != &other) {
            vkSampler = other.vkSampler;
            device = other.device;
        }
    }

    Sampler& operator=(const Sampler& other) = delete;
    Sampler& operator=(Sampler&& other) {
        if (this != &other) {
            std::swap(vkSampler, other.vkSampler);
            std::swap(device, other.device);
        }
        return *this;
    }

    ~Sampler() { vkDestroySampler(device->vkDevice, vkSampler, nullptr); }

private:
    vax::Device* device;
};

#endif