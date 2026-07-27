#pragma once

#include "baseRenderer.h"
#include "commandBuffer.h"
#include "drawableScene.h"
#include "frameTime.h"
#include "jfaPass.h"
#include "renderDestination.h"
#include "renderPassDescriptor.h"
#include "uiEngine.h"
#include "vkEngine.h"

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

    std::optional<vax::engine::JFAPass> _jfaPass;

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

    bool _drawGizmo(vax::vk::CommandBuffer& commandBuffer, vax::engine::DrawableScene* scene);

    void _resize();

    void _createRenderDestinations();

    void _writeFinalBlendDescriptorSets();

    void _mainPass(RenderPassInfo& renderPassInfo);

    void _roverCameraPass(RenderPassInfo& renderPassInfo);

    void _finalBlendPass(RenderPassInfo& renderPassInfo);

    void _roverCameraFBPass(RenderPassInfo& renderPassInfo);

    void _shadowSunPass(RenderPassInfo& renderPassInfo);

    bool _createRoverCameraRenderDestination();
};
} // namespace vax::engine