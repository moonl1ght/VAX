#pragma once

#include "luna.h"
#include "VKUtils.hpp"
#include "Logger.hpp"

namespace vax {
    class Device final {
    public:
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice vkDevice = VK_NULL_HANDLE;

        Device() {}
        ~Device() {
            LOG_INFO("Destroying device...");
            vkDestroyDevice(vkDevice, nullptr);
            vkDevice = VK_NULL_HANDLE;
            vkPhysicalDevice = VK_NULL_HANDLE;
            _isReady = false;
        }

        Device(const Device& rhs) = delete;
        Device(Device&& rhs) noexcept {
            std::swap(vkPhysicalDevice, rhs.vkPhysicalDevice);
            std::swap(vkDevice, rhs.vkDevice);
        }

        Device& operator=(const Device& rhs) = delete;
        Device& operator=(Device&& rhs) noexcept {
            if (this != &rhs) {
                std::swap(vkPhysicalDevice, rhs.vkPhysicalDevice);
                std::swap(vkDevice, rhs.vkDevice);
            }
            return *this;
        }

        bool load(VkInstance instance, VkSurfaceKHR surface, bool enableValidationLayers);
        bool isReady() { return _isReady; }

        VKUtils::QueueFamilyIndices getQueueFamilyIndices() const { return _indices; }

    private:
        VKUtils::QueueFamilyIndices _indices;
        bool _isReady = false;

        int createLogicalDevice(
            const VkPhysicalDevice& physicalDevice,
            const VkSurfaceKHR& surface,
            VkDevice& device,
            bool enableValidationLayers
        );
    };
}