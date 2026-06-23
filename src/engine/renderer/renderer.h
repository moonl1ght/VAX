#pragma once

#include "buffer.h"
#include "drawableScene.h"
#include "luna.h"
#include "uiEngine.h"
#include "vkEngine.h"

namespace vax::renderer {
class Renderer final {
  public:
    Renderer(vax::vk::Engine& vkEngine, vax::ui::UIEngine& uiEngine)
        : _vkEngine(vkEngine)
        , _uiEngine(uiEngine) {};

    ~Renderer() {};

    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

    bool render(vax::DrawableScene* scene, float deltaTime);
    void prepare(DrawableScene* scene);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("Renderer");

    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    std::reference_wrapper<vax::ui::UIEngine> _uiEngine;

    uint32_t _currentFrame = 0;

    bool _updateCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::DrawableScene* scene);
    bool _drawScene(VkCommandBuffer commandBuffer, vax::DrawableScene* scene, uint32_t imageIndex);
    void _drawUi(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void _setViewportAndScissor(VkCommandBuffer commandBuffer);
    bool _updateGlobalDescriptorSet(
        VkCommandBuffer commandBuffer, vax::DrawableScene* scene, VkPipelineLayout pipelineLayout
    );

    bool _drawGizmo(VkCommandBuffer commandBuffer, vax::DrawableScene* scene);
    bool _drawBackground(VkCommandBuffer commandBuffer, vax::DrawableScene* scene);
};
} // namespace vax::renderer