#pragma once

#include "luna.h"
#include "vkUtils.h"

namespace vax::vk {
    class Device final {
    public:
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice vkDevice = VK_NULL_HANDLE;

        Device() {
            _logger = Logger("Device");
        }

        Device(const Device& other) = delete;
        Device(Device&& other) = delete;
        Device& operator=(const Device& other) = delete;
        Device& operator=(Device&& other) = delete;

        bool load(VkInstance instance, VkSurfaceKHR surface, bool enableValidationLayers);
        void destroy();

        utils::QueueFamilyIndices getQueueFamilyIndices() const { return _indices; }

    private:
        Logger _logger;

        utils::QueueFamilyIndices _indices;

        int createLogicalDevice(
            const VkPhysicalDevice& physicalDevice,
            const VkSurfaceKHR& surface,
            VkDevice& device,
            bool enableValidationLayers
        );

        bool isDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

        int pickPhysicalDevice(
            const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& physicalDevice
        );
    };
}