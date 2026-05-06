#pragma once

#include "luna.h"

namespace vax {
    constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    constexpr int MAX_GLOBAL_TEXTURES = 1000;
    constexpr int MAX_MATERIALS = 1000;
}

#define VK_CHECK(x) \
    ({ \
        VkResult err = (x); \
        if (err != VK_SUCCESS) { \
            vax::utils::Logger::getInstance().error( \
                "Detected Vulkan error: {}", err \
            ); \
        } \
        (err == VK_SUCCESS); \
    })

namespace vax::vk::utils {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
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

}