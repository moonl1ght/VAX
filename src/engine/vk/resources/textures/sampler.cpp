#include "sampler.h"

using namespace vax::textures;
using namespace vax;

std::optional<Sampler> Sampler::createSampler(const vk::Device& device, std::string name) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device.vkPhysicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler vkSampler;
    if (vkCreateSampler(device.vkDevice, &samplerInfo, nullptr, &vkSampler) != VK_SUCCESS) {
        utils::Logger::getInstance().error("failed to create texture sampler!");
        return std::nullopt;
    }
    if (!name.empty()) {
        VkDebugUtilsObjectNameInfoEXT nameInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_SAMPLER,
            .objectHandle = reinterpret_cast<size_t>(vkSampler),
            .pObjectName = name.c_str(),
        };
        vax::vk::utils::pfnSetDebugUtilsObjectNameEXT(device.vkDevice, &nameInfo);
    }
    return Sampler(vkSampler, device);
}