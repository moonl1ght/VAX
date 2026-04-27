#pragma once

#include "luna.h"
#include "vkEngine.h"
#include "scene.h"
#include "buffer.h"
#include "imgui.h"

namespace vax {
    class Renderer final {
    public:
        Renderer(
            vax::vk::Engine& vkEngine
        )
            : _vkEngine(vkEngine) {
        };

        ~Renderer() {
            _sceneUniformBuffers.clear();
            _sceneUniformBuffersMapped.clear();
        };

        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
        Renderer(Renderer&& other) noexcept = delete;
        Renderer& operator=(Renderer&& other) noexcept = delete;

        bool render(vax::Scene* scene, float deltaTime, ImDrawData* imguiDrawData);
        void prepare();

    private:
        vax::utils::Logger _logger = vax::utils::Logger("Renderer");

        std::reference_wrapper<vax::vk::Engine> _vkEngine;

        std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
        std::vector<void*> _sceneUniformBuffersMapped;

        uint32_t _currentFrame = 0;

        // void drawBackground(VkCommandBuffer commandBuffer);
        bool recordCommandBuffer(
            VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::Scene* scene, float deltaTime, ImDrawData* imguiDrawData
        );
    };
}