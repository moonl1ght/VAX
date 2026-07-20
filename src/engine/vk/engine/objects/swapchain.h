#pragma once

#include "device.h"
#include "luna.h"
#include "size.h"
#include "vkUtils.h"

namespace vax::vk {
class Swapchain final {
  public:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent = {0, 0};

    explicit Swapchain(const vax::vk::Device& device, VkSurfaceKHR surface, vax::math::SizeUI size)
        : _surface(surface)
        , _size(size)
        , _device(device) {};

    Swapchain(const Swapchain& other) = delete;
    Swapchain(Swapchain&& other) = delete;
    Swapchain& operator=(const Swapchain& other) = delete;
    Swapchain& operator=(Swapchain&& other) = delete;

    bool setup();
    void cleanup();
    bool recreate();

  private:
    vax::Logger _logger = vax::Logger("Swapchain");
    vax::math::SizeUI _size;

    std::reference_wrapper<const vax::vk::Device> _device;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    bool createSwapchain();
    bool createImageViews();

    VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};
} // namespace vax::vk