#ifndef VKUtils_hpp
#define VKUtils_hpp

#include "luna.h"

namespace VKUtils {

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

    uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkImageView createImageView(
        VkDevice& device, 
        VkImage image, 
        VkFormat format, 
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
    );

    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkFormat findSupportedFormat(
        VkPhysicalDevice physicalDevice, 
        const std::vector<VkFormat>& candidates, 
        VkImageTiling tiling, 
        VkFormatFeatureFlags features
    );
    
    VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

} // namespace VKUtils

#endif