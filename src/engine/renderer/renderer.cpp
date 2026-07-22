#include "renderer.h"
#include "descriptorSetManager.h"
#include "imageUtils.h"
#include "imgui_impl_vulkan.h"
#include "pipeline.h"
#include "profiler.h"
#include "renderDestinationBuilder.h"
#include "renderPass.h"
#include "renderPassDescriptorBuilder.h"
#include "textureFactory.h"
#include "vkEngine.h"

using namespace vax::engine;
using namespace vax;
using namespace vax::vk;

void Renderer::setup() {
    auto swapchain = _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain();
    _mainRenderPassDescriptor =
        RenderPassDescriptorBuilder(*_vkEngine.get().device).buildMainOffscreen(swapchain->swapchainImageFormat, false);
    if (!_mainRenderPassDescriptor.has_value()) {
        _logger.error("Failed to create render pass descriptor!");
        return;
    }

    _swapchainRenderPassDescriptor =
        RenderPassDescriptorBuilder(*_vkEngine.get().device).buildMainSwapchain(swapchain->swapchainImageFormat);
    if (!_swapchainRenderPassDescriptor.has_value()) {
        _logger.error("Failed to create swapchain render pass descriptor!");
        return;
    }

    _createRenderDestinations();
    _uiEngine.get().setup(_swapchainRenderPassDescriptor->getVkRenderPass());
    _vkEngine.get().pipelineManager->setup(*_mainRenderPassDescriptor, *_swapchainRenderPassDescriptor);

    _jfaPass = std::make_optional<JFAPass>(
        *_vkEngine.get().device, *_vkEngine.get().descriptorSetManager, _vkEngine.get().allocator
    );
    _jfaPass->setup(_mainRenderDestination->maskTextures(), _mainRenderDestination->depthTexture());
}

void Renderer::prepare(DrawableScene* scene) {
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        if (scene != nullptr) {
            scene->prepareForDraw(engine::RenderCallContext{.currentFrame = i});
        }
        auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::GLOBAL, "global", "global", true
        );
        if (!globalDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get global descriptor set writer!");
            return;
        }
        scene->writeGlobalDescriptorSet(*globalDescriptorSetHandler);
        globalDescriptorSetHandler->update();
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "per_frame", "per_frame", true
        );
        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get frame descriptor set writer!");
            return;
        }
        auto roverCameraDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "rover_camera", "per_frame", true
        );
        if (!roverCameraDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get rover camera descriptor set writer!");
            return;
        }
        scene->writeFrameDescriptorSet(*frameDescriptorSetHandler, *roverCameraDescriptorSetHandler);
        roverCameraDescriptorSetHandler->update();
        frameDescriptorSetHandler->update();
    }
    _writeFinalBlendDescriptorSets();
}

void Renderer::_writeFinalBlendDescriptorSets() {
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND, "final_blend", "final_blend_sampled", true
        );
        auto inputMaskDescriptorSetHandler0 = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND, "fb_input_mask_0", "final_blend", true
        );
        auto inputMaskDescriptorSetHandler1 = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND, "fb_input_mask_1", "final_blend", true
        );
        if (!finalBlendDescriptorSetHandler.has_value() || !inputMaskDescriptorSetHandler0.has_value() ||
            !inputMaskDescriptorSetHandler1.has_value()) {
            _logger.error("Failed to get post process descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->writeTexture(_mainRenderDestination->textures()[i], 0, 0, true);
        finalBlendDescriptorSetHandler->writeTexture(_mainRenderDestination->maskTextures()[i], 1, 0, false);
        finalBlendDescriptorSetHandler->writeTexture(_mainRenderDestination->depthTexture(), 2, 0, false);
        finalBlendDescriptorSetHandler->update();
        inputMaskDescriptorSetHandler0->writeTexture(_jfaPass->outputATextures()[i], 0, 0, false);
        inputMaskDescriptorSetHandler0->update();
        inputMaskDescriptorSetHandler1->writeTexture(_jfaPass->outputBTextures()[i], 0, 0, false);
        inputMaskDescriptorSetHandler1->update();
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
        _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchain,
        UINT64_MAX,
        _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );
    bool drawSecondaryWindow = false;
    if (scene != nullptr) {
        drawSecondaryWindow = scene->shouldDrawSecondaryWindow();
    }
    uint32_t roverCameraImageIndex = UINT32_MAX;
    if (drawSecondaryWindow) {
        VkResult roverCameraResult = vkAcquireNextImageKHR(
            _vkEngine.get().device->vkDevice,
            _vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain()->swapchain,
            UINT64_MAX,
            _vkEngine.get().syncObjectsManager->getRoverCameraImageAvailableSemaphores()[_currentFrame],
            VK_NULL_HANDLE,
            &roverCameraImageIndex
        );
        if (roverCameraResult != VK_SUCCESS && roverCameraResult != VK_SUBOPTIMAL_KHR) {
            _logger.warning("Failed to acquire rover camera swap chain image, skipping secondary window this frame");
            drawSecondaryWindow = false;
            roverCameraImageIndex = UINT32_MAX;
        }
    }

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

    if (drawSecondaryWindow && !_roverCameraRenderDestination.has_value()) {
        if (!_createRoverCameraRenderDestination()) {
            _logger.error("Failed to create rover camera render destination!");
        }
    }

    auto updateResult = _updateCommandBuffer(commandBuffer, imageIndex, roverCameraImageIndex, scene);
    if (!updateResult) {
        _logger.error("Failed to update command buffer!");
        return false;
    }

    std::vector<VkSemaphore> waitSemaphores;
    waitSemaphores.push_back(_vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame]);
    if (drawSecondaryWindow) {
        waitSemaphores.push_back(
            _vkEngine.get().syncObjectsManager->getRoverCameraImageAvailableSemaphores()[_currentFrame]
        );
    }
    std::vector<VkPipelineStageFlags> waitStages(waitSemaphores.size(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    std::vector<VkSemaphore> signalSemaphores;
    signalSemaphores.push_back(_vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores()[_currentFrame]);
    if (drawSecondaryWindow) {
        signalSemaphores.push_back(
            _vkEngine.get().syncObjectsManager->getRoverCameraRenderFinishedSemaphores()[_currentFrame]
        );
    }
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &_vkEngine.get().commandManager->commandBuffers[_currentFrame],
        .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphores = signalSemaphores.data()
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

    std::vector<VkSwapchainKHR> swapChains;
    swapChains.push_back(_vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchain);
    std::vector<uint32_t> imageIndices;
    imageIndices.push_back(imageIndex);
    if (drawSecondaryWindow) {
        swapChains.push_back(_vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain()->swapchain);
        imageIndices.push_back(roverCameraImageIndex);
    }
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pWaitSemaphores = signalSemaphores.data(),
        .swapchainCount = static_cast<uint32_t>(swapChains.size()),
        .pSwapchains = swapChains.data(),
        .pImageIndices = imageIndices.data()
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
    VkCommandBuffer commandBuffer,
    uint32_t imageIndex,
    uint32_t roverCameraImageIndex,
    vax::engine::DrawableScene* scene
) {
    ZoneScopedN("Renderer::updateCommandBuffer");
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (!VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))) {
        _logger.error("Failed to begin recording command buffer!");
        return false;
    }

    _setMainViewportAndScissor(commandBuffer);

    if (scene != nullptr) {
        _drawScene(commandBuffer, scene, imageIndex, roverCameraImageIndex);
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
    auto swapchain = _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain();
    RenderPass renderPass(
        *_vkEngine.get().device,
        "draw_ui_render_pass",
        _swapchainRenderPassDescriptor->getVkRenderPass(),
        _swapchainRenderDestination->framebuffers[imageIndex],
        swapchain->swapchainExtent
    );
    renderPass.pass(commandBuffer, [&]() { _uiEngine.get().render(commandBuffer); });
}

bool Renderer::_drawScene(
    VkCommandBuffer commandBuffer,
    vax::engine::DrawableScene* scene,
    uint32_t imageIndex,
    uint32_t roverCameraImageIndex
) {
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
    if (scene->shouldDrawSecondaryWindow() && _roverCameraRenderDestination.has_value()) {
        _roverCameraPass(pipelineLayout, commandBuffer, scene, roverCameraImageIndex);
        _roverCameraFBPass(commandBuffer, scene, roverCameraImageIndex);
    }
    _setMainViewportAndScissor(commandBuffer);
    _jfaPass->execute(commandBuffer, _mainRenderDestination->maskTextures()[_currentFrame], _currentFrame);
    _finalBlendPass(commandBuffer, scene, imageIndex);
    return true;
}

void Renderer::_setMainViewportAndScissor(VkCommandBuffer commandBuffer) {
    auto swapchain = _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain();
    _setViewportAndScissor(commandBuffer, swapchain->swapchainExtent);
}

void Renderer::_setViewportAndScissor(VkCommandBuffer commandBuffer, VkExtent2D extent) {
    VkViewport viewport{
        .x = 0.0f,
        .y = static_cast<float>(extent.height),
        .width = static_cast<float>(extent.width),
        .height = -static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{.offset = {0, 0}, .extent = extent};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

bool Renderer::_updateGlobalDescriptorSet(
    VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, VkPipelineLayout pipelineLayout
) {
    auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
        _currentFrame, vax::vk::DescriptorSetManager::PoolType::GLOBAL, "global", "global", false
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
        .clearValue = {.depthStencil = {0.0f, 0}},
    };

    auto xOffset = static_cast<float>(
        _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchainExtent.width - 256
    );
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

void Renderer::_resize() {
    _createRenderDestinations();
    _jfaPass->writeTextures(_mainRenderDestination->maskTextures(), _mainRenderDestination->depthTexture());
    _writeFinalBlendDescriptorSets();
}

void Renderer::_createRenderDestinations() {
    _mainRenderDestination =
        RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator)
            .buildMainOffscreen(
                *_vkEngine.get().commandManager,
                _vkEngine.get().queueManager->graphicsQueue,
                _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchainExtent,
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
                                          *_vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain(),
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
        _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchainExtent,
        _mainRenderPassDescriptor->colorAttachmentCount
    );
    renderPass.pass(commandBuffer, [&]() {
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "per_frame", "per_frame", false
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
    _setMainViewportAndScissor(commandBuffer);
    RenderPass renderPass(
        *_vkEngine.get().device,
        "swapchain_render_pass",
        _swapchainRenderPassDescriptor->getVkRenderPass(),
        _swapchainRenderDestination->framebuffers[imageIndex],
        _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain()->swapchainExtent
    );
    renderPass.pass(commandBuffer, [&]() {
        auto pipelineLayout =
            _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::FINAL_BLEND);
        if (!pipelineLayout) {
            _logger.error("Failed to get final blend pipeline layout!");
            return;
        }
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            "final_blend",
            "final_blend_sampled",
            false
        );
        auto maskDescriptorSetName = _jfaPass->isFinalImageA() ? "fb_input_mask_0" : "fb_input_mask_1";
        auto maskDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            maskDescriptorSetName,
            "final_blend",
            false
        );
        auto perFrameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "per_frame", "per_frame", false
        );
        if (!finalBlendDescriptorSetHandler.has_value() || !maskDescriptorSetHandler.has_value() ||
            !perFrameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get default descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->bind(commandBuffer, pipelineLayout, 0);
        maskDescriptorSetHandler->bind(commandBuffer, pipelineLayout, 1);
        perFrameDescriptorSetHandler->bind(commandBuffer, pipelineLayout, 2);
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

void Renderer::_roverCameraFBPass(
    VkCommandBuffer commandBuffer, vax::engine::DrawableScene* scene, uint32_t imageIndex
) {
    auto extent = _vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain()->swapchainExtent;
    _setViewportAndScissor(commandBuffer, extent);
    RenderPass renderPass(
        *_vkEngine.get().device,
        "rover_camera_fb_render_pass",
        _swapchainRenderPassDescriptor->getVkRenderPass(),
        _roverCameraFBRenderDestination->framebuffers[imageIndex],
        extent
    );
    renderPass.pass(commandBuffer, [&]() {
        auto pipelineLayout =
            _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::ROVER_CAMERA_FB);
        if (!pipelineLayout) {
            _logger.error("Failed to get final blend pipeline layout!");
            return;
        }
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            "rover_camera_fb",
            "final_blend_cam_sampled",
            false
        );
        if (!finalBlendDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get rover camera FB descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->bind(commandBuffer, pipelineLayout, 0);
        auto pipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::ROVER_CAMERA_FB);
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
    });
}

void Renderer::_roverCameraPass(
    VkPipelineLayout pipelineLayout,
    VkCommandBuffer commandBuffer,
    vax::engine::DrawableScene* scene,
    uint32_t imageIndex
) {
    auto roverCameraExtent =
        _vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain()->swapchainExtent;
    RenderPass renderPass(
        *_vkEngine.get().device,
        "rover_camera_render_pass",
        _mainRenderPassDescriptor->getVkRenderPass(),
        _roverCameraRenderDestination->framebuffers[_currentFrame],
        roverCameraExtent,
        _mainRenderPassDescriptor->colorAttachmentCount
    );
    _setViewportAndScissor(commandBuffer, roverCameraExtent);
    renderPass.pass(commandBuffer, [&]() {
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            _currentFrame, vax::vk::DescriptorSetManager::PoolType::PER_FRAME, "rover_camera", "per_frame", false
        );

        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get rover camera descriptor set writer!");
            return;
        }
        frameDescriptorSetHandler->bind(commandBuffer, pipelineLayout, MainSetIndices::PER_FRAME_SET_INDEX);

        auto pipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::ROVER_CAMERA);
        if (!pipeline) {
            _logger.error("Failed to get rover camera pipeline!");
            return;
        }
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkPipeline);
        DrawContext drawContext{
            .commandBuffer = commandBuffer,
            .pipelineLayout = pipeline->vkPipelineLayout,
            .currentFrame = _currentFrame,
        };
        scene->draw(drawContext);
    });
}

bool Renderer::_createRoverCameraRenderDestination() {
    _roverCameraRenderDestination =
        RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator)
            .buildMainOffscreen(
                *_vkEngine.get().commandManager,
                _vkEngine.get().queueManager->graphicsQueue,
                _vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain()->swapchainExtent,
                *_mainRenderPassDescriptor
            );
    if (!_roverCameraRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return false;
    }
    _roverCameraFBRenderDestination =
        RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator)
            .buildMainSwapchain(
                *_vkEngine.get().commandManager,
                _vkEngine.get().queueManager->graphicsQueue,
                *_vkEngine.get().getWindowController().getSecondaryWindow()->getSwapchain(),
                *_swapchainRenderPassDescriptor
            );
    if (!_roverCameraFBRenderDestination.has_value()) {
        _logger.error("Failed to create swapchain render destination!");
        return false;
    }
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; i++) {
        auto roverCameraFBDescriptorSetHandler = _vkEngine.get().descriptorSetManager->getDescriptorSetHandler(
            i, vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND, "rover_camera_fb", "final_blend_cam_sampled", true
        );
        roverCameraFBDescriptorSetHandler->writeTexture(_roverCameraRenderDestination->textures()[i], 0, 0, true);
        roverCameraFBDescriptorSetHandler->update();
    }
    return true;
}