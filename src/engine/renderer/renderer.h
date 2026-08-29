#pragma once

#include "baseRenderer.h"
#include "commandBuffer.h"
#include "drawableScene.h"
#include "frameTime.h"
#include "jfaPass.h"
#include "renderPassDescriptor.h"
#include "uiEngine.h"
#include "vkEngine.h"

#include "renderPass_V2.h"
#include "renderPassGraph.h"

namespace vax::engine {
class Renderer final : public BaseRenderer {
  public:
    Renderer(vax::vk::Engine& vkEngine, vax::ui::UIEngine& uiEngine)
        : BaseRenderer(vkEngine, uiEngine) {};

    ~Renderer() {};

    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

    bool render(DrawableScene* scene, const FrameTime& frameTime);
    void setup();
    void prepare(DrawableScene* scene);

  private:
    struct RenderPassInfo {
        vax::vk::CommandBuffer* commandBuffer;
        vax::vk::RenderPassDescriptor* renderPassDescriptor;
        VkPipelineLayout pipelineLayout;
        vax::engine::DrawableScene* scene;
        uint32_t imageIndex;
    };

    vax::Logger _logger = vax::Logger("Renderer");

    std::unique_ptr<vax::engine::RenderPassGraph> _renderPassGraph;

    // std::optional<vax::engine::JFAPass> _jfaPass;

    // std::optional<vax::engine::RenderPass_V2> _shadowPass;

    // std::optional<vax::engine::RenderPass_V2> _mainPass;

    // std::optional<vax::engine::RenderPass_V2> _roverCameraFBPass;

    // std::optional<vax::engine::RenderPass_V2> _roverCameraMainPass;

    bool _updateCommandBuffer(
        vax::vk::CommandBuffer& commandBuffer,
        uint32_t imageIndex,
        uint32_t roverCameraImageIndex,
        vax::engine::DrawableScene* scene
    );

    bool _drawScene(
        vax::vk::CommandBuffer& commandBuffer,
        vax::engine::DrawableScene* scene,
        uint32_t imageIndex,
        uint32_t roverCameraImageIndex
    );

    void _drawUi(vax::vk::CommandBuffer& commandBuffer, uint32_t imageIndex);

    bool _bindGlobalDescriptorSet(vax::vk::CommandBuffer& commandBuffer, VkPipelineLayout pipelineLayout);

    void _resize();

    // void _createRenderDestinations();

    void _writeFinalBlendDescriptorSets();

    // void _finalBlendPass(RenderPassInfo& renderPassInfo);

    bool _createRoverCameraRenderDestination();
};
} // namespace vax::engine