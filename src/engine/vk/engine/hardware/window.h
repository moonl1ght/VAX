#pragma once

#include "luna.h"

namespace vax::vk {
    class Window final {
    public:
        uint32_t width = 800;
        uint32_t height = 600;
        SDL_Window* window = nullptr;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        explicit Window(
            uint32_t width = 800,
            uint32_t height = 600
        ) noexcept
            : width(width)
            , height(height)
        { };

        Window(const Window& other) = delete;
        Window(Window&& other) = delete;
        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) = delete;

        bool load();
        bool createSurface(VkInstance instance);
        bool destroySurface();
        bool destroyWindow();
        bool destroy();

    private:
        VkInstance _instance = VK_NULL_HANDLE;
        Logger _logger = Logger("Window");
    };
};
