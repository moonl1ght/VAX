#pragma once

#include "luna.h"
#include "vkEngine.h"
#include "window.h"
#include "imgui.h"

namespace vax::ui {
    class UILayer final {
    public:
        UILayer(
            vax::vk::Engine& vkEngine,
            vax::vk::Window& window
        ) : _vkEngine(vkEngine)
            , _window(window)
        { }

        ~UILayer() {}

        UILayer(const UILayer& other) = delete;
        UILayer& operator=(const UILayer& other) = delete;
        UILayer(UILayer&& other) noexcept = delete;
        UILayer& operator=(UILayer&& other) noexcept = delete;

        void update();
        void render(VkCommandBuffer commandBuffer);
        void setup();
        void cleanup();
        void processEvents(SDL_Event& event);

    private:
        ImDrawData* _imguiDrawData = nullptr;
        VkDescriptorPool _imguiDescriptorPool = VK_NULL_HANDLE;

        std::reference_wrapper<vax::vk::Window> _window;
        std::reference_wrapper<vax::vk::Engine> _vkEngine;
    };
}