#include "swapchain.h"
#include "ImageUtils.hpp"
#include "queueManager.h"

using namespace vax::vk;

bool Swapchain::setup() {
    if (!createSwapchain()) return false;
    if (!createImageViews()) return false;
    return true;
}

void Swapchain::cleanup() {
    for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(_device.get().vkDevice, imageView, nullptr);
    }

    for (auto image : swapchainImages) {
        vkDestroyImage(_device.get().vkDevice, image, nullptr);
    }

    vkDestroySwapchainKHR(_device.get().vkDevice, swapchain, nullptr);
}

bool Swapchain::recreate() {
    cleanup();

    if (!setup()) return false;

    return true;
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (
            availableFormat.format ==
            VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            ) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        SDL_GetWindowSizeInPixels(_window.get().window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(
            actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width
        );
        actualExtent.height = std::clamp(
            actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height
        );

        return actualExtent;
    }
}

bool Swapchain::createSwapchain() {
    utils::SwapChainSupportDetails swapChainSupport = utils::querySwapChainSupport(
        _device.get().vkPhysicalDevice, _window.get().surface
    );

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _window.get().surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    utils::QueueFamilyIndices indices = utils::findQueueFamilies(
        _device.get().vkPhysicalDevice, _window.get().surface
    );
    if (!indices.isComplete()) {
        _logger.error("Queue family indices are not complete!");
        return false;
    }

    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (!VK_CHECK(vkCreateSwapchainKHR(_device.get().vkDevice, &createInfo, nullptr, &swapchain))) {
        _logger.error("Failed to create swap chain!");
        return false;
    }

    vkGetSwapchainImagesKHR(_device.get().vkDevice, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    if (!VK_CHECK(vkGetSwapchainImagesKHR(_device.get().vkDevice, swapchain, &imageCount, swapchainImages.data()))) {
        _logger.error("Failed to get swap chain images!");
        return false;
    }

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    return true;
}

bool Swapchain::createImageViews() {
    swapchainImageViews.resize(swapchainImages.size());

    for (uint32_t i = 0; i < swapchainImages.size(); i++) {
        auto swapchainImageView = vax::createImageView(
            _device.get().vkDevice, swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT
        );
        if (swapchainImageView) {
            swapchainImageViews[i] = *swapchainImageView;
        }
        else {
            _logger.error("Failed to create swap chain image view!");
            return false;
        }
    }

    return true;
}