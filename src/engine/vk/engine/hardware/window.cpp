#include "window.h"

using namespace vax::vk;

bool Window::load() {
    if (!SDL_Vulkan_LoadLibrary(NULL)) {
        _logger.error("Failed to load Vulkan library: {}", SDL_GetError());
        return false;
    }
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    window = SDL_CreateWindow(
        "Luna",
        width,
        height,
        window_flags
    );
    if (window == nullptr) {
        _logger.error("Failed to create window");
        return false;
    }
    return true;
}

bool Window::createSurface(VkInstance instance) {
    _instance = instance;
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        _logger.error("Failed to create surface");
        return false;
    }
    return true;
}

void Window::destroySurface() {
    if (surface != VK_NULL_HANDLE && _instance != VK_NULL_HANDLE) {
        _logger.info("Destroying surface...");
        vkDestroySurfaceKHR(_instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
        _instance = VK_NULL_HANDLE;
    }
}

void Window::destroyWindow() {
    if (window != nullptr) {
        _logger.info("Destroying window...");
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void Window::destroy() {
    destroyWindow();
    destroySurface();
}