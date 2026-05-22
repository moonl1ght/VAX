#include "window.h"

using namespace vax::vk;
using namespace vax;

bool vax::vk::Window::load() {
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

bool vax::vk::Window::createSurface(VkInstance instance) {
    _instance = instance;
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        _logger.error("Failed to create surface");
        return false;
    }
    return true;
}

void vax::vk::Window::destroySurface() {
    if (surface != VK_NULL_HANDLE && _instance != VK_NULL_HANDLE) {
        _logger.info("Destroying surface...");
        vkDestroySurfaceKHR(_instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
        _instance = VK_NULL_HANDLE;
    }
}

void vax::vk::Window::destroyWindow() {
    if (window != nullptr) {
        _logger.info("Destroying window...");
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void vax::vk::Window::destroy() {
    destroyWindow();
    destroySurface();
}