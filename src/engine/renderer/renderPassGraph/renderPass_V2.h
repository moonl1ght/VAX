#pragma once

#include "commandBuffer.h"
#include "device.h"
#include "drawableScene.h"
#include "pipeline.h"
#include "renderDestination.h"
#include "renderPassDescriptor.h"
#include "renderPassNode.h"

namespace vax::engine {
class RenderPass_V2 : public RenderPassNode {
  public:
    RenderPass_V2(
        std::string_view id,
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        std::string_view debugName,
        std::weak_ptr<vax::vk::RenderDestination> renderDestination,
        std::weak_ptr<vax::vk::RenderPassDescriptor> renderDescriptor
    )
        : RenderPassNode(id)
        , _device(device)
        , _pipelineManager(pipelineManager)
        , _descriptorSetManager(descriptorSetManager)
        , _logger(vax::Logger(std::string(debugName)))
        , _debugName(debugName)
        , _renderDestination(std::move(renderDestination))
        , _renderDescriptor(std::move(renderDescriptor)) {
        if (auto renderDescriptorShared = _renderDescriptor.lock()) {
            _clearValues.assign(
                renderDescriptorShared->colorAttachmentCount, VkClearValue{.color = {{0.0f, 0.0f, 0.0f, 0.0f}}}
            );
            _clearValues.push_back(VkClearValue{.depthStencil = {0.0f, 0}});
        }
    };

    RenderPass_V2(const RenderPass_V2&) = delete;
    RenderPass_V2(RenderPass_V2&&) = default;
    RenderPass_V2& operator=(const RenderPass_V2&) = delete;
    RenderPass_V2& operator=(RenderPass_V2&&) = default;

    virtual ~RenderPass_V2() = default;

    virtual void runPass(RunPassInfo& runPassInfo);

    void setRenderArea(VkRect2D renderArea) { _renderArea = renderArea; }

    void setSwapchainExtent(VkExtent2D swapchainExtent) { _swapchainExtent = swapchainExtent; }

    void addInputDescriptorSet(InputDescriptorSetInfo inputDescriptorSetInfo) {
        _inputDescriptorSetInfos.push_back(inputDescriptorSetInfo);
    }

    void updateInputDescriptorSetAt(size_t index, InputDescriptorSetInfo inputDescriptorSetInfo) {
        _inputDescriptorSetInfos[index] = inputDescriptorSetInfo;
    }

    const InputDescriptorSetInfo& getInputDescriptorSetAt(size_t index) const {
        return _inputDescriptorSetInfos[index];
    }

    InputDescriptorSetInfo& getInputDescriptorSetAt(size_t index) {
        return _inputDescriptorSetInfos[index];
    }

    void setPipeline(vax::vk::PipelineName pipelineName) { _pipelineName = pipelineName; }

    void setPipelineLayout(vax::vk::PipelineLayoutName pipelineLayoutName) { _pipelineLayoutName = pipelineLayoutName; }

    void setDrawWork(std::function<void(RunPassInfo&, DrawContext&)> drawWork) { _drawWork = drawWork; }

    void setRenderToSwapchain(bool renderToSwapchain) { _renderToSwapchain = renderToSwapchain; }

  protected:
    vax::Logger _logger;
    std::reference_wrapper<vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::PipelineManager> _pipelineManager;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;

    std::weak_ptr<vax::vk::RenderDestination> _renderDestination;
    std::weak_ptr<vax::vk::RenderPassDescriptor> _renderDescriptor;

    std::vector<InputDescriptorSetInfo> _inputDescriptorSetInfos;

    std::string _debugName;
    std::vector<VkClearValue> _clearValues;
    std::function<void(RunPassInfo&, DrawContext&)> _drawWork;
    VkExtent2D _swapchainExtent = {0, 0};
    VkRect2D _renderArea{};
    vax::vk::PipelineName _pipelineName = vax::vk::PipelineName::UNKNOWN;
    vax::vk::PipelineLayoutName _pipelineLayoutName = vax::vk::PipelineLayoutName::UNKNOWN;
    bool _renderToSwapchain = false;

    template <typename Work> void _pass(RunPassInfo& runPassInfo, Work work) {
        auto renderDestinationShared = _renderDestination.lock();
        if (!renderDestinationShared) {
            _logger.error("Render destination not found!");
            return;
        }
        auto renderDescriptorShared = _renderDescriptor.lock();
        if (!renderDescriptorShared) {
            _logger.error("Render descriptor not found!");
            return;
        }
        auto& framebuffers = renderDestinationShared->framebuffers;
        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderDescriptorShared->getVkRenderPass(),
            .framebuffer = framebuffers[_renderToSwapchain ? runPassInfo.imageIndex : runPassInfo.frameIndex],
            .renderArea = _renderArea,
            .clearValueCount = static_cast<uint32_t>(_clearValues.size()),
            .pClearValues = _clearValues.data(),
        };
        if (!_debugName.empty()) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = VK_OBJECT_TYPE_RENDER_PASS,
                .objectHandle = reinterpret_cast<size_t>(renderDescriptorShared->getVkRenderPass()),
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