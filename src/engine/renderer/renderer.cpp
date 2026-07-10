#include "renderer.h"
#include "descriptorSetManager.h"
#include "imageUtils.h"
#include "imgui_impl_vulkan.h"
#include "pipeline.h"
#include "profiler.h"
#include "renderDestinationBuilder.h"
#include "renderPass.h"
#include "renderPassDescriptorBuilder.h"
#include "vkEngine.h"

using namespace vax::engine;
using namespace vax;
using namespace vax::vk;

void Renderer::setup() {
    _mainRenderPassDescriptor = RenderPassDescriptorBuilder(*_vkEngine.get().device)
                                    .buildMainOffscreen(_vkEngine.get().swapchain->swapchainImageFormat, false);
    if (!_mainRenderPassDescriptor.has_value()) {
        _logger.error("Failed to create render pass descriptor!");
        return;
    }

    _swapchainRenderPassDescriptor = RenderPassDescriptorBuilder(*_vkEngine.get().device)
                                         .buildMainSwapchain(_vkEngine.get().swapchain->swapchainImageFormat);
    if (!_swapchainRenderPassDescriptor.has_value()) {
        _logger.error("Failed to create swapchain render pass descriptor!");
        return;
    }
    _createRenderDestinations();
    _uiEngine.get().setup(_swapchainRenderPassDescriptor->getVkRenderPass());
    _vkEngine.get().pipelineManager->setup(*_mainRenderPassDescriptor, *_swapchainRenderPassDescriptor);
}

void Renderer::prepare(DrawableScene* scene) {
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        if (scene != nullptr) {
            scene->prepareForDraw(engine::RenderCallContext{.currentFrame = i});
        }
        auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetLayout::SetType::GLOBAL
        );
        if (!globalDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get global descriptor set writer!");
            return;
        }
        scene->writeGlobalDescriptorSet(*globalDescriptorSetHandler);
        globalDescriptorSetHandler->update();
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetLayout::SetType::PER_FRAME
        );
        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get frame descriptor set writer!");
            return;
        }
        scene->writeFrameDescriptorSet(*frameDescriptorSetHandler);
        frameDescriptorSetHandler->update();
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetLayout::SetType::FINAL_BLEND
        );
        if (!finalBlendDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get post process descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->writeTexture(
            _mainRenderDestination->textures()[i], FinalBlendBindingIndices::FINAL_BLEND_TEXTURE_INDEX, 0, true
        );
        finalBlendDescriptorSetHandler->update();
    }
}

bool Renderer::render(DrawableScene* scene, const FrameTime& frameTime) {
    ZoneScopedN("Renderer::render");
    if (scene != nullptr) {
        scene->prepareForDraw(engine::RenderCallContext{.currentFrame = _currentFrame});
    }

    vkWaitForFences(
        _vkEngine.get().device->vkDevice,
        1,
        &_vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame],
        VK_TRUE,
        UINT64_MAX
    );

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        _vkEngine.get().device->vkDevice,
        _vkEngine.get().swapchain->swapchain,
        UINT64_MAX,
        _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vkEngine.get().resize();
        _resize();
        if (scene != nullptr) {
            scene->resize();
        }
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        _logger.error("Failed to acquire swap chain image!");
        return false;
    }

    vkResetFences(
        _vkEngine.get().device->vkDevice, 1, &_vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame]
    );

    auto commandBuffer = _vkEngine.get().commandManager->commandBuffers[_currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);

    auto updateResult = _updateCommandBuffer(commandBuffer, imageIndex, scene);
    if (!updateResult) {
        _logger.error("Failed to update command buffer!");
        return false;
    }

    VkSemaphore waitSemaphores[] = {_vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {_vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores()[_currentFrame]};
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &_vkEngine.get().commandManager->commandBuffers[_currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    if (!VK_CHECK(vkQueueSubmit(
            _vkEngine.get().queueManager->graphicsQueue,
            1,
            &submitInfo,
            _vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame]
        ))) {
        _logger.error("failed to submit draw command buffer!");
        return false;
    }

    VkSwapchainKHR swapChains[] = {_vkEngine.get().swapchain->swapchain};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex
    };

    result = vkQueuePresentKHR(_vkEngine.get().queueManager->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _vkEngine.get().framebufferResized) {
        _vkEngine.get().framebufferResized = false;
        _vkEngine.get().resize();
        _resize();
        if (scene != nullptr) {
            scene->resize();
        }
        return false;
    } else if (result != VK_SUCCESS) {
        _logger.error("failed to present swap chain image!");
        return false;
    }

    _currentFrame = (_currentFrame + 1) % vax::vk::MAX_FRAMES_IN_FLIGHT;
    return true;
}

bool Renderer::_updateCommandBuffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex, vax::engine::DrawableScene* scene
) {
    ZoneScopedN("Renderer::updateCommandBuffer");
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (!VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))) {
        _logger.error("Failed to begin recording command buffer!");
        return false;
    }

    _setViewportAndScissor(commandBuffer);

    if (scene != nullptr) {
        _drawScene(commandBuffer, scene, imageIndex);
    } else {
        _drawUi(commandBuffer, imageIndex);
    }

    if (!VK_CHECK(vkEndCommandBuffer(commandBuffer))) {
        _logger.error("Failed to end command buffer!");
        return false;
    }
    return true;
}

void Renderer::_drawUi(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    RenderPass renderPass(
        *_vkEngine.get().device,
        "draw_ui_render_pass",
        _swapchainRenderPassDescriptor->getVkRenderPass(),
        _swapchainRenderDestination->framebuffers[imageIndex],
        _vkEngine.get().swapchain->swapchainExtent
    );
    renderPass.pass(commandBuffer, [&]() { _uiEngine.get().render(commandBuffer); });
}

bool Renderer::_drawScene(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, uint32_t imageIndex) {
    auto pipelineLayout = _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    if (!pipelineLayout) {
        _logger.error("Failed to get base pipeline layout!");
        return false;
    }
    if (!_updateGlobalDescriptorSet(commandBuffer, scene, pipelineLayout)) {
        _logger.error("Failed to update global descriptor set!");
        return false;
    }

    _mainPass(pipelineLayout, commandBuffer, scene, imageIndex);
    _finalBlendPass(commandBuffer, scene, imageIndex);
    return true;
}

void Renderer::_setViewportAndScissor(VkCommandBuffer commandBuffer) {
    VkViewport viewport{
        .x = 0.0f,
        .y = static_cast<float>(_vkEngine.get().swapchain->swapchainExtent.height),
        .width = static_cast<float>(_vkEngine.get().swapchain->swapchainExtent.width),
        .height = -static_cast<float>(_vkEngine.get().swapchain->swapchainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{.offset = {0, 0}, .extent = _vkEngine.get().swapchain->swapchainExtent};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

bool Renderer::_updateGlobalDescriptorSet(
    VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, VkPipelineLayout pipelineLayout
) {
    auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
        _currentFrame, vax::vk::DescriptorSetLayout::SetType::GLOBAL
    );
    if (!globalDescriptorSetHandler.has_value()) {
        return false;
    }
    globalDescriptorSetHandler->bind(commandBuffer, pipelineLayout, MainSetIndices::GLOBAL_SET_INDEX);
    return true;
}

bool Renderer::_drawGizmo(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene) {
    VkClearAttachment clearAttachment{
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .clearValue = {.depthStencil = {1.0f, 0}},
    };

    auto xOffset = static_cast<float>(_vkEngine.get().swapchain->swapchainExtent.width - 256);
    VkClearRect clearRect{
        .rect = {.offset = {static_cast<int32_t>(xOffset), 0}, .extent = {256, 256}},
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkViewport viewport{
        .x = xOffset,
        .y = 256.0f,
        .width = 256.0f,
        .height = -256.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor{
        .offset = {static_cast<int32_t>(xOffset), 0},
        .extent = {256, 256},
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
    auto gizmoPipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::BASE);
    if (!gizmoPipeline) {
        _logger.error("Failed to get gizmo pipeline!");
        return false;
    }
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gizmoPipeline->vkPipeline);
    DrawContext drawContext{
        .commandBuffer = commandBuffer,
        .pipelineLayout = gizmoPipeline->vkPipelineLayout,
        .currentFrame = _currentFrame,
    };
    scene->drawGizmo(drawContext);
    return true;
}

void Renderer::_resize() { _createRenderDestinations(); }

void Renderer::_createRenderDestinations() {
    _mainRenderDestination = RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator)
                                 .buildMainOffscreen(
                                     *_vkEngine.get().commandManager,
                                     _vkEngine.get().queueManager->graphicsQueue,
                                     _vkEngine.get().swapchain->swapchainExtent,
                                     *_mainRenderPassDescriptor
                                 );
    if (!_mainRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return;
    }
    _swapchainRenderDestination = RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator)
                                      .buildMainSwapchain(
                                          *_vkEngine.get().commandManager,
                                          _vkEngine.get().queueManager->graphicsQueue,
                                          *_vkEngine.get().swapchain,
                                          *_swapchainRenderPassDescriptor
                                      );
    if (!_swapchainRenderDestination.has_value()) {
        _logger.error("Failed to create swapchain render destination!");
        return;
    }
}

void Renderer::_mainPass(
    VkPipelineLayout pipelineLayout,
    VkCommandBuffer commandBuffer,
    vax::engine::DrawableScene* scene,
    uint32_t imageIndex
) {
    RenderPass renderPass(
        *_vkEngine.get().device,
        "main_render_pass",
        _mainRenderPassDescriptor->getVkRenderPass(),
        _mainRenderDestination->framebuffers[_currentFrame],
        _vkEngine.get().swapchain->swapchainExtent,
        _mainRenderPassDescriptor->colorAttachmentCount
    );
    renderPass.pass(commandBuffer, [&]() {
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame, vax::vk::DescriptorSetLayout::SetType::PER_FRAME
        );

        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get default descriptor set writer!");
            return;
        }
        frameDescriptorSetHandler->bind(commandBuffer, pipelineLayout, MainSetIndices::PER_FRAME_SET_INDEX);

        auto pipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::PBR);
        if (!pipeline) {
            _logger.error("Failed to get PBR pipeline!");
            return;
        }
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
        DrawContext drawContext{
            .commandBuffer = commandBuffer,
            .pipelineLayout = pipeline->vkPipelineLayout,
            .currentFrame = _currentFrame,
        };
        scene->draw(drawContext);

        if (!_drawGizmo(commandBuffer, scene)) {
            _logger.error("Failed to draw gizmo!");
        }
    });
}

void Renderer::_finalBlendPass(VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, uint32_t imageIndex) {
    _setViewportAndScissor(commandBuffer);
    RenderPass renderPass(
        *_vkEngine.get().device,
        "swapchain_render_pass",
        _swapchainRenderPassDescriptor->getVkRenderPass(),
        _swapchainRenderDestination->framebuffers[imageIndex],
        _vkEngine.get().swapchain->swapchainExtent
    );
    renderPass.pass(commandBuffer, [&]() {
        auto pipelineLayout =
            _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::FINAL_BLEND);
        if (!pipelineLayout) {
            _logger.error("Failed to get final blend pipeline layout!");
            return;
        }
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame, vax::vk::DescriptorSetLayout::SetType::FINAL_BLEND
        );
        if (!finalBlendDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get default descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->bind(commandBuffer, pipelineLayout, 0);
        auto pipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::FINAL_BLEND);
        if (!pipeline) {
            _logger.error("Failed to get final blend pipeline!");
            return;
        }
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);

        DrawContext drawContext{
            .commandBuffer = commandBuffer,
            .pipelineLayout = pipeline->vkPipelineLayout,
            .currentFrame = _currentFrame,
        };
        scene->drawBackground(drawContext);

        _uiEngine.get().render(commandBuffer);
    });
}