#pragma once

#include "luna.h"
#include "device.h"
#include "vkUtils.h"
#include "window.h"

namespace vax::vk {
    class SwapchainManager final {
    public:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D swapchainExtent = { 0, 0 };

        explicit SwapchainManager(
            const vax::vk::Window& window,
            const vax::vk::Device& device
        ) noexcept
            : _window(window)
            , _device(device) {
        };

        SwapchainManager(const SwapchainManager& other) = delete;
        SwapchainManager(SwapchainManager&& other) = delete;
        SwapchainManager& operator=(const SwapchainManager& other) = delete;
        SwapchainManager& operator=(SwapchainManager&& other) = delete;

        bool setup();
        void cleanup();
        bool recreate();

    private:
        Logger _logger = Logger("SwapchainManager");

        std::reference_wrapper<const vax::vk::Window> _window;
        std::reference_wrapper<const vax::vk::Device> _device;

        bool createSwapchain();
        bool createImageViews();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}