#pragma once

#include "commandBuffer.h"
#include "device.h"
#include "drawableScene.h"
#include "renderDestination.h"
#include "renderPassDescriptor.h"

namespace vax::engine {
class RenderPass_V2 {
  public:
    struct RunPassInfo final {
        vax::vk::CommandBuffer& commandBuffer;
        vax::engine::DrawableScene* scene;
        VkRect2D renderArea;
        uint32_t imageIndex;
        uint32_t frameIndex;
    };

    RenderPass_V2(
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        std::string_view debugName,
        std::shared_ptr<vax::vk::RenderDestination> renderDestination,
        std::shared_ptr<vax::vk::RenderPassDescriptor> renderDescriptor
    )
        : _device(device)
        , _pipelineManager(pipelineManager)
        , _descriptorSetManager(descriptorSetManager)
        , _debugName(debugName)
        , _renderDestination(renderDestination)
        , _renderDescriptor(renderDescriptor) {
        _clearValues.assign(_renderDescriptor->colorAttachmentCount, VkClearValue{.color = {{0.0f, 0.0f, 0.0f, 0.0f}}});
        _clearValues.push_back(VkClearValue{.depthStencil = {0.0f, 0}});
    };

    virtual ~RenderPass_V2() = default;

    virtual void runPass(RunPassInfo& runPassInfo) = 0;

  protected:
    std::reference_wrapper<vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::PipelineManager> _pipelineManager;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;
    std::string _debugName;
    std::shared_ptr<vax::vk::RenderDestination> _renderDestination;
    std::shared_ptr<vax::vk::RenderPassDescriptor> _renderDescriptor;
    std::vector<VkClearValue> _clearValues;

    template <typename Work> void _pass(RunPassInfo& runPassInfo, Work work) {
        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = _renderDescriptor->getVkRenderPass(),
            .framebuffer = _renderDestination->framebuffers[runPassInfo.frameIndex],
            .renderArea = runPassInfo.renderArea,
            .clearValueCount = static_cast<uint32_t>(_clearValues.size()),
            .pClearValues = _clearValues.data(),
        };
        if (!_debugName.empty()) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VK_OBJECT_TYPE_RENDER_PASS,
                .objectHandle = reinterpret_cast<size_t>(_renderDescriptor->getVkRenderPass()),
                .pObjectName = _debugName.data(),
            };
            vax::vk::pfnSetDebugUtilsObjectNameEXT(_device.get().vkDevice, &nameInfo);

            VkDebugUtilsLabelEXT label{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                .pLabelName = _debugName.data(),
            };
            vax::vk::pfnCmdBeginDebugUtilsLabelEXT(runPassInfo.commandBuffer.vkCommandBuffer, &label);
        }
        vkCmdBeginRenderPass(runPassInfo.commandBuffer.vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        work();
        vkCmdEndRenderPass(runPassInfo.commandBuffer.vkCommandBuffer);
        if (!_debugName.empty()) {
            vax::vk::pfnCmdEndDebugUtilsLabelEXT(runPassInfo.commandBuffer.vkCommandBuffer);
        }
    }

    void _setViewportAndScissor(vax::vk::CommandBuffer& commandBuffer, VkExtent2D extent);
};
} // namespace vax::engine