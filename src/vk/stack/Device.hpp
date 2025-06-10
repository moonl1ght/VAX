#ifndef Device_hpp
#define Device_hpp

#include "luna.h"
#include "VKUtils.hpp"
#include <iostream>

namespace vax {
class Device final {
public:
    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice vkDevice = VK_NULL_HANDLE;

    Device() { }
    ~Device() {
        vkDestroyDevice(vkDevice, nullptr);
        vkDevice = VK_NULL_HANDLE;
        _isReady = false;
    }

    Device(const Device &rhs) = delete;
    Device(Device &&rhs) noexcept {
        std::swap(vkPhysicalDevice, rhs.vkPhysicalDevice);
        std::swap(vkDevice, rhs.vkDevice);
    }

    Device &operator=(const Device &rhs) = delete;
    Device &operator=(Device &&rhs) noexcept {
        if (this != &rhs) {
            std::swap(vkPhysicalDevice, rhs.vkPhysicalDevice);
            std::swap(vkDevice, rhs.vkDevice);
        }
        return *this;
    }

    bool load(VkInstance instance, VkSurfaceKHR surface, bool enableValidationLayers);
    bool isReady() { return _isReady; }

private:
    bool _isReady = false;
};
} // namespace vax

#endif