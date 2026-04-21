#include "sampler.h"

using namespace vax::textures;

std::optional<Sampler> Sampler::createSampler(const vax::vk::Device& device)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device.vkPhysicalDevice, &properties);

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
    if (vkCreateSampler(device.vkDevice, &samplerInfo, nullptr, &vkSampler) != VK_SUCCESS) {
        vax::utils::Logger::getInstance().error("failed to create texture sampler!");
        return std::nullopt;
    }
    return vax::textures::Sampler(vkSampler, device);
}