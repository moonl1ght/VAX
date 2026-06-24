#pragma once

#include "luna.h"
#include "shaderUniforms.h"

namespace vax {
constexpr int MAX_FRAMES_IN_FLIGHT = 2;
constexpr int MAX_GLOBAL_TEXTURES = MAX_TEXTURES;
constexpr int MAX_MATERIALS = 500;
constexpr int MAX_GLOBAL_SAMPLERS = MAX_SAMPLERS;
constexpr int MAX_INSTANCE_BUFFERS = 1000;
} // namespace vax

#define VK_CHECK(x)                                                                                                    \
    ({                                                                                                                 \
        VkResult err = (x);                                                                                            \
        if (err != VK_SUCCESS) {                                                                                       \
            vax::utils::Logger::getInstance().error("Detected Vulkan error: {}", err);                                 \
        }                                                                                                              \
        (err == VK_SUCCESS);                                                                                           \
    })

namespace vax::vk::utils {

inline PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT = nullptr;

inline void setPfnSetDebugUtilsObjectNameEXT(VkInstance instance) {
    if (pfnSetDebugUtilsObjectNameEXT == nullptr) {
        pfnSetDebugUtilsObjectNameEXT =
            (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
    }
}

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

VkFormat findSupportedFormat(
    VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features
);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
} // namespace vax::vk::utils