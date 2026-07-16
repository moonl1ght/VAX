#pragma once

#include "buffer.h"
#include "drawableScene.h"
#include "frameTime.h"
#include "luna.h"
#include "renderDestination.h"
#include "renderPassDescriptor.h"
#include "uiEngine.h"
#include "vkEngine.h"
#include "jfaPass.h"

namespace vax::engine {
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

    bool render(DrawableScene* scene, const FrameTime& frameTime);
    void setup();
    void prepare(DrawableScene* scene);

  private:
    vax::Logger _logger = vax::Logger("Renderer");

    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    std::reference_wrapper<vax::ui::UIEngine> _uiEngine;

    std::optional<vax::vk::RenderPassDescriptor> _mainRenderPassDescriptor;
    std::optional<vax::vk::RenderPassDescriptor> _swapchainRenderPassDescriptor;

    std::optional<vax::vk::RenderDestination> _mainRenderDestination;
    std::optional<vax::vk::RenderDestination> _swapchainRenderDestination;

    std::optional<vax::engine::JFAPass> _jfaPass;

    uint32_t _currentFrame = 0;

    bool _updateCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::engine::DrawableScene* scene);
    bool _drawScene(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, uint32_t imageIndex);
    void _drawUi(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void _setViewportAndScissor(VkCommandBuffer commandBuffer);
    bool _updateGlobalDescriptorSet(
        VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, VkPipelineLayout pipelineLayout
    );

    bool _drawGizmo(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene);

    void _resize();
    void _createRenderDestinations();
    void _writeFinalBlendDescriptorSets();

    void _mainPass(
        VkPipelineLayout pipelineLayout,
        VkCommandBuffer commandBuffer,
        vax::engine::DrawableScene* scene,
        uint32_t imageIndex
    );
    void _finalBlendPass(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, uint32_t imageIndex);
};
} // namespace vax::engine