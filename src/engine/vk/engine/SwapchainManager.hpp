#pragma once

#include "luna.h"
#include "Device.hpp"
#include "VKUtils.hpp"

class SwapchainManager final {
public:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent = { 0, 0 };

    SwapchainManager(
        SDL_Window* window,
        VkSurfaceKHR surface,
        vax::Device* device
    )
        : _window(window)
        , _surface(surface)
        , _device(device) {
    };

    bool setup();
    void cleanup();
    bool recreate();

private:
    SDL_Window* _window;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    vax::Device* _device;

    bool createSwapchain();
    bool createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};