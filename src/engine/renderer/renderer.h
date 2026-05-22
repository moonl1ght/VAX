#pragma once

#include "buffer.h"
#include "drawableScene.h"
#include "luna.h"
#include "uiLayer.h"
#include "vkEngine.h"

namespace vax::renderer {
class Renderer final {
  public:
    Renderer(vax::vk::Engine& vkEngine, vax::ui::UILayer& uiLayer)
        : _vkEngine(vkEngine)
        , _uiLayer(uiLayer) {};

    ~Renderer() {};

    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

    bool render(vax::DrawableScene* scene, float deltaTime);
    void prepare();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("Renderer");

    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    std::reference_wrapper<vax::ui::UILayer> _uiLayer;

    uint32_t _currentFrame = 0;

    bool _updateCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::DrawableScene* scene);

    void _setViewportAndScissor(VkCommandBuffer commandBuffer);
    bool _updateGlobalDescriptorSet(
        VkCommandBuffer commandBuffer, vax::DrawableScene* scene, VkPipelineLayout pipelineLayout
    );

    bool _drawGizmo(VkCommandBuffer commandBuffer, vax::DrawableScene* scene);
    bool _drawBackground(VkCommandBuffer commandBuffer, vax::DrawableScene* scene);
};
} // namespace vax::renderer