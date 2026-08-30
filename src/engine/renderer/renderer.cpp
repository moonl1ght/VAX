#include "renderer.h"
#include "descriptorSetManager.h"
#include "imageUtils.h"
#include "imgui_impl_vulkan.h"
#include "jfaPass.h"
#include "pipeline.h"
#include "profiler.h"
#include "textureFactory.h"
#include "vkEngine.h"

using namespace vax::engine;
using namespace vax;
using namespace vax::vk;

void Renderer::setup() {
    _roverDemoPassGraphManager = std::make_unique<RenderPassGraphManager>(
        _vkEngine.get().allocator,
        *_vkEngine.get().device,
        *_vkEngine.get().pipelineManager,
        *_vkEngine.get().descriptorSetManager,
        _vkEngine.get().getWindowController(),
        _uiEngine.get()
    );
    auto swapchain = _getSwapchain(0);
    _roverDemoPassGraphManager->setupRenderPassDescriptors(swapchain->swapchainImageFormat);
    auto swapchainRenderPassDescriptor = _roverDemoPassGraphManager->getRenderPassDescriptor("swapchain");
    _uiEngine.get().setup(swapchainRenderPassDescriptor->getVkRenderPass());

    _roverDemoPassGraphManager->setupRenderDestinations(
        *_vkEngine.get().commandManager, *_vkEngine.get().queueManager, *swapchain
    );
    _rebuildRenderPassGraph(false);

    _uiPassGraph = _roverDemoPassGraphManager->buildUiGraph();
}

void Renderer::prepare(DrawableScene* scene) {
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        if (scene != nullptr) {
            scene->prepareForDraw(engine::RenderCallContext{.currentFrame = i});
        }
        auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::GLOBAL,
            vax::vk::DescriptorSetManager::SetLayoutName::GLOBAL,
            "global",
            true
        );
        if (!globalDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get global descriptor set writer!");
            return;
        }
        scene->writeGlobalDescriptorSet(*globalDescriptorSetHandler);
        auto shadowSunRenderDestination = _renderPassGraph->getRenderDestination("shadow_sun");
        globalDescriptorSetHandler->writeTexture(
            shadowSunRenderDestination->depthTexture(), GlobalBindingIndices::GLOBAL_SHADOW_TEXTURE_INDEX
        );
        globalDescriptorSetHandler->update();
        auto frameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
            vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
            "per_frame",
            true
        );
        if (!frameDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get frame descriptor set writer!");
            return;
        }
        auto roverCameraDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
            vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
            "rover_camera",
            true
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

void Renderer::_writeRoverCameraDescriptorSets() {
    auto roverCameraRenderDestination = _renderPassGraph->getRenderDestination("rover_camera_main");
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto roverCameraFBDescriptorSetHandler =
            _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
                i,
                vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
                vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND_SIMPLE,
                "rover_camera_fb",
                true
            );
        roverCameraFBDescriptorSetHandler->writeTexture(roverCameraRenderDestination->textures()[i], 0, 0, true);
        roverCameraFBDescriptorSetHandler->update();
    }
}

void Renderer::_writeFinalBlendDescriptorSets() {
    auto mainRenderDestination = _renderPassGraph->getRenderDestination("main");
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto finalBlendDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND,
            "final_blend",
            true
        );
        if (!finalBlendDescriptorSetHandler.has_value()) {
            _logger.error("Failed to get post process descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->textures()[i], 0, 0, true);
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->maskTextures()[i], 1, 0, false);
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->depthTexture(), 2, 0, false);
        finalBlendDescriptorSetHandler->update();
    }
}

void Renderer::_rebuildRenderPassGraph(bool withRoverCamera) {
    _renderPassGraph = _roverDemoPassGraphManager->buildRoverDemoGraph(withRoverCamera);
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto inputMaskDescriptorSetHandler0 = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            vax::vk::DescriptorSetManager::SetLayoutName::SINGLE_STORAGE_IMAGE,
            "fb_input_mask_0",
            true
        );
        auto inputMaskDescriptorSetHandler1 = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
            i,
            vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
            vax::vk::DescriptorSetManager::SetLayoutName::SINGLE_STORAGE_IMAGE,
            "fb_input_mask_1",
            true
        );

        auto jfaNode = _renderPassGraph->findNode("jfa_pass");
        auto jfaPass = std::dynamic_pointer_cast<JFAPass>(jfaNode);
        inputMaskDescriptorSetHandler0->writeTexture(jfaPass->outputATextures()[i], 0, 0, false);
        inputMaskDescriptorSetHandler0->update();
        inputMaskDescriptorSetHandler1->writeTexture(jfaPass->outputBTextures()[i], 0, 0, false);
        inputMaskDescriptorSetHandler1->update();
    }
}

bool Renderer::render(DrawableScene* scene, const FrameTime& frameTime) {
    ZoneScopedN("Renderer::render");
    if (scene != nullptr) {
        scene->prepareForDraw(engine::RenderCallContext{.currentFrame = _currentFrame});

        if (_wasResized) {
            prepare(scene);
            _wasResized = false;
        }
    }

    _waitForFence();

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(
        _vkEngine.get().device->vkDevice,
        _vkEngine.get().getWindowController().getWindow(0)->getSwapchain()->swapchain,
        UINT64_MAX,
        _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores("main")[_currentFrame],
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
            _vkEngine.get().getWindowController().getWindow(1)->getSwapchain()->swapchain,
            UINT64_MAX,
            _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores("secondary")[_currentFrame],
            VK_NULL_HANDLE,
            &roverCameraImageIndex
        );
        if (roverCameraResult != VK_SUCCESS && roverCameraResult != VK_SUBOPTIMAL_KHR) {
            _logger.warning("Failed to acquire rover camera swap chain image, skipping secondary window this frame");
            drawSecondaryWindow = false;
            roverCameraImageIndex = UINT32_MAX;
        }
    }

    _outputImageIndices = {imageIndex, roverCameraImageIndex};

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vkEngine.get().resize();
        _resize();
        if (scene != nullptr) {
            scene->resize();
        }
        return false;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        _logger.error("Failed to acquire swap chain image!");
        return false;
    }

    vkResetFences(
        _vkEngine.get().device->vkDevice, 1, &_vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame]
    );

    auto& commandBuffer = _vkEngine.get().commandManager->getCommandBuffer(_currentFrame);
    commandBuffer.reset();

    if (drawSecondaryWindow && _rendererMode == RendererMode::ROVER_DEMO_WITHOUT_ROVER_CAMERA) {
        _rendererMode = RendererMode::ROVER_DEMO_WITH_ROVER_CAMERA;
        _roverDemoPassGraphManager->setupRenderDestinationsForRoverCamera(
            *_vkEngine.get().commandManager, *_vkEngine.get().queueManager, *_getSwapchain(1)
        );
        _rebuildRenderPassGraph(true);
        _writeRoverCameraDescriptorSets();
    } else if (!drawSecondaryWindow && _rendererMode == RendererMode::ROVER_DEMO_WITH_ROVER_CAMERA) {
        _rendererMode = RendererMode::ROVER_DEMO_WITHOUT_ROVER_CAMERA;
        _rebuildRenderPassGraph(false);
    }

    auto updateResult = _updateCommandBuffer(commandBuffer, imageIndex, roverCameraImageIndex, scene);
    if (!updateResult) {
        _logger.error("Failed to update command buffer!");
        return false;
    }

    std::vector<VkSemaphore> waitSemaphores;
    waitSemaphores.push_back(_vkEngine.get().syncObjectsManager->getImageAvailableSemaphores("main")[_currentFrame]);
    if (drawSecondaryWindow) {
        waitSemaphores.push_back(
            _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores("secondary")[_currentFrame]
        );
    }
    std::vector<VkPipelineStageFlags> waitStages(waitSemaphores.size(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    std::vector<VkSemaphore> signalSemaphores;
    signalSemaphores.push_back(_vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores("main")[_currentFrame]);
    if (drawSecondaryWindow) {
        signalSemaphores.push_back(
            _vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores("secondary")[_currentFrame]
        );
    }
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer.vkCommandBuffer,
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
    swapChains.push_back(_getSwapchain(0)->swapchain);
    std::vector<uint32_t> imageIndices;
    imageIndices.push_back(imageIndex);
    if (drawSecondaryWindow) {
        swapChains.push_back(_getSwapchain(1)->swapchain);
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
    CommandBuffer& commandBuffer, uint32_t imageIndex, uint32_t roverCameraImageIndex, vax::engine::DrawableScene* scene
) {
    ZoneScopedN("Renderer::updateCommandBuffer");
    if (!commandBuffer.begin())
        return false;

    if (scene != nullptr) {
        _drawScene(commandBuffer, scene);
    } else {
        _drawUi(commandBuffer);
    }

    if (!commandBuffer.end())
        return false;
    return true;
}

void Renderer::_drawUi(CommandBuffer& commandBuffer) {
    RenderPassNode::RunPassInfo runPassInfo{
        .commandBuffer = commandBuffer,
        .scene = nullptr,
        .imageIndices = _outputImageIndices,
        .frameIndex = _currentFrame,
    };

    _uiPassGraph->run(runPassInfo);
}

bool Renderer::_drawScene(CommandBuffer& commandBuffer, DrawableScene* scene) {
    auto pipelineLayout = _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    if (!pipelineLayout) {
        _logger.error("Failed to get base pipeline layout!");
        return false;
    }
    if (!_bindGlobalDescriptorSet(commandBuffer, pipelineLayout))
        return false;

    RenderPassNode::RunPassInfo runPassInfo{
        .commandBuffer = commandBuffer,
        .scene = scene,
        .imageIndices = _outputImageIndices,
        .frameIndex = _currentFrame,
    };

    _renderPassGraph->run(runPassInfo);

    return true;
}

bool Renderer::_bindGlobalDescriptorSet(CommandBuffer& commandBuffer, VkPipelineLayout pipelineLayout) {
    auto globalDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
        _currentFrame,
        vax::vk::DescriptorSetManager::PoolType::GLOBAL,
        vax::vk::DescriptorSetManager::SetLayoutName::GLOBAL,
        "global",
        false
    );
    if (!globalDescriptorSetHandler.has_value()) {
        _logger.error("Failed to get global descriptor set writer!");
        return false;
    }
    globalDescriptorSetHandler->bind(commandBuffer.vkCommandBuffer, pipelineLayout, MainSetIndices::GLOBAL_SET_INDEX);
    return true;
}

void Renderer::_resize() {
    auto swapchain = _getSwapchain(0);
    _roverDemoPassGraphManager->setupRenderDestinations(
        *_vkEngine.get().commandManager, *_vkEngine.get().queueManager, *swapchain
    );
    if (_rendererMode == RendererMode::ROVER_DEMO_WITH_ROVER_CAMERA) {
        _roverDemoPassGraphManager->setupRenderDestinationsForRoverCamera(
            *_vkEngine.get().commandManager, *_vkEngine.get().queueManager, *_getSwapchain(1)
        );
        _rebuildRenderPassGraph(true);
        _writeRoverCameraDescriptorSets();
    } else {
        _rebuildRenderPassGraph(false);
    }

    _uiPassGraph = _roverDemoPassGraphManager->buildUiGraph();
    _wasResized = true;
}