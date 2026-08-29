#include "renderer.h"
#include "descriptorSetManager.h"
#include "imageUtils.h"
#include "imgui_impl_vulkan.h"
#include "pipeline.h"
#include "profiler.h"
#include "renderDestinationBuilder.h"
#include "renderPass.h"
#include "renderPassDescriptorBuilder.h"
#include "renderPass_V2.h"
#include "textureFactory.h"
#include "vkEngine.h"

#include "renderPassGraphFactory.h"

using namespace vax::engine;
using namespace vax;
using namespace vax::vk;

void Renderer::setup() {
    // auto swapchain = _getSwapchain(0);
    // auto renderPassDescriptorBuilder = RenderPassDescriptorBuilder(*_vkEngine.get().device);
    // auto mainRenderPassDescriptor = renderPassDescriptorBuilder.buildOffscreen(swapchain->swapchainImageFormat,
    // false); auto shadowSunRenderPassDescriptor =
    //     renderPassDescriptorBuilder.buildShadowSun(swapchain->swapchainImageFormat, false);
    // auto swapchainRenderPassDescriptor = renderPassDescriptorBuilder.buildSwapchain(swapchain->swapchainImageFormat);
    // if (!mainRenderPassDescriptor.has_value() || !shadowSunRenderPassDescriptor.has_value() ||
    //     !swapchainRenderPassDescriptor.has_value()) {
    //     _logger.error("Failed to create main render pass descriptor!");
    //     return;
    // }

    // _uiEngine.get().setup(swapchainRenderPassDescriptor->getVkRenderPass());
    // _vkEngine.get().pipelineManager->setup(
    //     *mainRenderPassDescriptor, *swapchainRenderPassDescriptor, *shadowSunRenderPassDescriptor
    // );

    // auto mainRenderPassDescriptorShared =
    //     std::make_shared<vax::vk::RenderPassDescriptor>(std::move(mainRenderPassDescriptor.value()));
    // auto shadowSunRenderPassDescriptorShared =
    //     std::make_shared<vax::vk::RenderPassDescriptor>(std::move(shadowSunRenderPassDescriptor.value()));
    // auto swapchainRenderPassDescriptorShared =
    //     std::make_shared<vax::vk::RenderPassDescriptor>(std::move(swapchainRenderPassDescriptor.value()));

    // _renderPassDescriptors.emplace("main", mainRenderPassDescriptorShared);
    // _renderPassDescriptors.emplace("shadow_sun", shadowSunRenderPassDescriptorShared);
    // _renderPassDescriptors.emplace("swapchain", swapchainRenderPassDescriptorShared);

    // _createRenderDestinations();
    // if (!_renderDestinations.contains("main")) {
    //     _logger.error("Main render destination not found!");
    //     return;
    // }
    // auto& mainRenderDestination = _renderDestinations.at("main");
    // _jfaPass = std::make_optional<JFAPass>(
    //     "jfa_pass", *_vkEngine.get().device, *_vkEngine.get().descriptorSetManager, _vkEngine.get().allocator
    // );
    // _jfaPass->setup(mainRenderDestination);

    // _shadowPass = std::make_optional<RenderPass_V2>(
    //     "shadow_pass",
    //     *_vkEngine.get().device,
    //     *_vkEngine.get().pipelineManager,
    //     *_vkEngine.get().descriptorSetManager,
    //     "ShadowPass",
    //     _renderDestinations.at("shadow_sun"),
    //     _renderPassDescriptors.at("shadow_sun")
    // );
    // _shadowPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = _getSwapchain(0)->swapchainExtent});
    // _shadowPass->setPipeline(vax::vk::PipelineName::SHADOW);
    // _shadowPass->setPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    // _shadowPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
    //     runPassInfo.scene->draw(drawContext);
    // });
    // auto dynamicOffset =
    //     static_cast<uint32_t>(_vkEngine.get().device->minUniformBufferOffsetAlignment<UniformBufferObject>());
    // _shadowPass->addInputDescriptorSet({
    //     .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
    //     .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
    //     .name = "per_frame",
    //     .bindingInfo = {
    //     .setIndex = MainSetIndices::PER_FRAME_SET_INDEX,
    //     .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     .dynamicOffsetCount = 1,
    //     .dynamicOffsets = {dynamicOffset},
    //     },
    // });
    // _shadowPass->setSwapchainExtent(_getSwapchain(0)->swapchainExtent);

    // _mainPass = std::make_optional<RenderPass_V2>(
    //     "main_pass",
    //     *_vkEngine.get().device,
    //     *_vkEngine.get().pipelineManager,
    //     *_vkEngine.get().descriptorSetManager,
    //     "MainPass",
    //     _renderDestinations.at("main"),
    //     _renderPassDescriptors.at("main")
    // );
    // _mainPass->addInputDescriptorSet({
    //     .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
    //     .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
    //     .name = "per_frame",
    //     .bindingInfo = {
    //     .setIndex = MainSetIndices::PER_FRAME_SET_INDEX,
    //     .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     .dynamicOffsetCount = 1,
    //     .dynamicOffsets = {0},
    //     },
    // });
    // _mainPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = _getSwapchain(0)->swapchainExtent});
    // _mainPass->setSwapchainExtent(_getSwapchain(0)->swapchainExtent);
    // _mainPass->setPipeline(vax::vk::PipelineName::PBR);
    // _mainPass->setPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    // _mainPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
    //     runPassInfo.scene->draw(drawContext);
    // });

    auto renderPassGraphFactory = RenderPassGraphFactory(
        _vkEngine.get().allocator,
        *_vkEngine.get().device,
        *_vkEngine.get().pipelineManager,
        *_vkEngine.get().descriptorSetManager,
        _vkEngine.get().getWindowController(),
        _uiEngine.get()
    );
    auto swapchain = _getSwapchain(0);
    renderPassGraphFactory.setupRenderPassDescriptors(swapchain->swapchainImageFormat);
    renderPassGraphFactory.setupRenderDestinations(
        *_vkEngine.get().commandManager, *_vkEngine.get().queueManager, *swapchain
    );
    _renderPassGraph = renderPassGraphFactory.buildRoverDemoGraph();

    // auto swapchainRenderPassDescriptor = _renderPassGraph->getRenderPassDescriptor("swapchain");
    // auto shadowSunRenderPassDescriptor = _renderPassGraph->getRenderPassDescriptor("shadow_sun");
    // auto mainRenderPassDescriptor = _renderPassGraph->getRenderPassDescriptor("main");
    // _vkEngine.get().pipelineManager->setup(
    //     *mainRenderPassDescriptor, *swapchainRenderPassDescriptor, *shadowSunRenderPassDescriptor
    // );
    // _uiEngine.get().setup(swapchainRenderPassDescriptor->getVkRenderPass());
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

bool Renderer::_createRoverCameraRenderDestination() {
    // auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    // auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");
    // auto renderDestinationBuilder = RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator);
    // auto roverCameraRenderDestination = renderDestinationBuilder.buildMainOffscreen(
    //     *_vkEngine.get().commandManager,
    //     _vkEngine.get().queueManager->graphicsQueue,
    //     _getSwapchain(1)->swapchainExtent,
    //     *mainRenderPassDescriptor
    // );
    // if (!roverCameraRenderDestination.has_value()) {
    //     _logger.error("Failed to create main render destination!");
    //     return false;
    // }
    // auto roverCameraFBRenderDestination = renderDestinationBuilder.buildSwapchain(
    //     *_vkEngine.get().commandManager,
    //     _vkEngine.get().queueManager->graphicsQueue,
    //     *_getSwapchain(1),
    //     *swapchainRenderPassDescriptor
    // );
    // if (!roverCameraFBRenderDestination.has_value()) {
    //     _logger.error("Failed to create rover camera swapchain render destination!");
    //     return false;
    // }
    // for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
    //     auto roverCameraFBDescriptorSetHandler =
    //         _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
    //             i,
    //             vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
    //             vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND_SIMPLE,
    //             "rover_camera_fb",
    //             true
    //         );
    //     roverCameraFBDescriptorSetHandler->writeTexture(roverCameraRenderDestination->textures()[i], 0, 0, true);
    //     roverCameraFBDescriptorSetHandler->update();
    // }

    // auto roverCameraRenderDestinationShared =
    //     std::make_shared<vax::vk::RenderDestination>(std::move(roverCameraRenderDestination.value()));
    // auto roverCameraFBRenderDestinationShared =
    //     std::make_shared<vax::vk::RenderDestination>(std::move(roverCameraFBRenderDestination.value()));
    // _renderDestinations.emplace("rover_camera_main", roverCameraRenderDestinationShared);
    // _renderDestinations.emplace("rover_camera_swapchain", roverCameraFBRenderDestinationShared);

    // _roverCameraFBPass = std::make_optional<RenderPass_V2>(
    //     "rover_camera_fb_pass",
    //     *_vkEngine.get().device,
    //     *_vkEngine.get().pipelineManager,
    //     *_vkEngine.get().descriptorSetManager,
    //     "RoverCameraFBPass",
    //     _renderDestinations.at("rover_camera_swapchain"),
    //     _renderPassDescriptors.at("swapchain")
    // );
    // _roverCameraFBPass->addInputDescriptorSet({
    //     .poolType = vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
    //     .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND_SIMPLE,
    //     .name = "rover_camera_fb",
    //     .bindingInfo = {
    //     .setIndex = 0,
    //     .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     .dynamicOffsetCount = 1,
    //     .dynamicOffsets = {0},
    //     },
    // });
    // _roverCameraFBPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = _getSwapchain(1)->swapchainExtent});
    // _roverCameraFBPass->setSwapchainExtent(_getSwapchain(1)->swapchainExtent);
    // _roverCameraFBPass->setPipeline(vax::vk::PipelineName::ROVER_CAMERA_FB);
    // _roverCameraFBPass->setPipelineLayout(vax::vk::PipelineLayoutName::ROVER_CAMERA_FB);
    // _roverCameraFBPass->setRenderToSwapchain(true);
    // _roverCameraFBPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
    //     runPassInfo.scene->drawBackground(drawContext);
    // });

    // _roverCameraMainPass = std::make_optional<RenderPass_V2>(
    //     "rover_camera_main_pass",
    //     *_vkEngine.get().device,
    //     *_vkEngine.get().pipelineManager,
    //     *_vkEngine.get().descriptorSetManager,
    //     "RoverCameraMainPass",
    //     _renderDestinations.at("rover_camera_main"),
    //     _renderPassDescriptors.at("main")
    // );
    // _roverCameraMainPass->addInputDescriptorSet({
    //     .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
    //     .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
    //     .name = "rover_camera",
    //     .bindingInfo = {
    //     .setIndex = MainSetIndices::PER_FRAME_SET_INDEX,
    //     .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     .dynamicOffsetCount = 1,
    //     .dynamicOffsets = {0},
    //     },
    // });
    // _roverCameraMainPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = _getSwapchain(1)->swapchainExtent});
    // _roverCameraMainPass->setSwapchainExtent(_getSwapchain(1)->swapchainExtent);
    // _roverCameraMainPass->setPipeline(vax::vk::PipelineName::ROVER_CAMERA);
    // _roverCameraMainPass->setPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    // _roverCameraMainPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
    //     runPassInfo.scene->draw(drawContext);
    // });
    return true;
}

// void Renderer::_createRenderDestinations() {
//     auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
//     auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");
//     auto& shadowSunRenderPassDescriptor = _renderPassDescriptors.at("shadow_sun");
//     auto renderDestinationBuilder = RenderDestinationBuilder(*_vkEngine.get().device, _vkEngine.get().allocator);
//     auto mainRenderDestination = renderDestinationBuilder.buildMainOffscreen(
//         *_vkEngine.get().commandManager,
//         _vkEngine.get().queueManager->graphicsQueue,
//         _getSwapchain(0)->swapchainExtent,
//         *mainRenderPassDescriptor
//     );
//     if (!mainRenderDestination.has_value()) {
//         _logger.error("Failed to create main render destination!");
//         return;
//     }
//     auto mainRenderDestinationShared =
//         std::make_shared<vax::vk::RenderDestination>(std::move(mainRenderDestination.value()));
//     _renderDestinations.emplace("main", mainRenderDestinationShared);
//     auto swapchainRenderDestination = renderDestinationBuilder.buildSwapchain(
//         *_vkEngine.get().commandManager,
//         _vkEngine.get().queueManager->graphicsQueue,
//         *_getSwapchain(0),
//         *swapchainRenderPassDescriptor
//     );
//     if (!swapchainRenderDestination.has_value()) {
//         _logger.error("Failed to create swapchain render destination!");
//         return;
//     }
//     auto swapchainRenderDestinationShared =
//         std::make_shared<vax::vk::RenderDestination>(std::move(swapchainRenderDestination.value()));
//     _renderDestinations.emplace("swapchain", swapchainRenderDestinationShared);

//     auto shadowSunRenderDestination = renderDestinationBuilder.buildShadowSunOffscreen(
//         *_vkEngine.get().commandManager,
//         _vkEngine.get().queueManager->graphicsQueue,
//         _getSwapchain(0)->swapchainExtent,
//         *shadowSunRenderPassDescriptor
//     );
//     if (!shadowSunRenderDestination.has_value()) {
//         _logger.error("Failed to create shadow sun render destination!");
//         return;
//     }
//     auto shadowSunRenderDestinationShared =
//         std::make_shared<vax::vk::RenderDestination>(std::move(shadowSunRenderDestination.value()));
//     _renderDestinations.emplace("shadow_sun", shadowSunRenderDestinationShared);
// }

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
        if (!finalBlendDescriptorSetHandler.has_value() || !inputMaskDescriptorSetHandler0.has_value() ||
            !inputMaskDescriptorSetHandler1.has_value()) {
            _logger.error("Failed to get post process descriptor set writer!");
            return;
        }
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->textures()[i], 0, 0, true);
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->maskTextures()[i], 1, 0, false);
        finalBlendDescriptorSetHandler->writeTexture(mainRenderDestination->depthTexture(), 2, 0, false);
        finalBlendDescriptorSetHandler->update();
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

    // if (drawSecondaryWindow && !_renderDestinations.contains("rover_camera_main")) {
    //     if (!_createRoverCameraRenderDestination()) {
    //         _logger.error("Failed to create rover camera render destination!");
    //     }
    // }

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

    _setViewportAndScissor(commandBuffer, _getSwapchain(0)->swapchainExtent);

    if (scene != nullptr) {
        _drawScene(commandBuffer, scene, imageIndex, roverCameraImageIndex);
    } else {
        _drawUi(commandBuffer, imageIndex);
    }

    if (!commandBuffer.end())
        return false;
    return true;
}

void Renderer::_drawUi(CommandBuffer& commandBuffer, uint32_t imageIndex) {
    auto swapchainRenderPassDescriptor = _renderPassGraph->getRenderPassDescriptor("swapchain");
    auto swapchainRenderDestination = _renderPassGraph->getRenderDestination("swapchain");
    auto swapchain = _getSwapchain(0);
    RenderPass renderPass(
        *_vkEngine.get().device,
        "draw_ui_render_pass",
        swapchainRenderPassDescriptor->getVkRenderPass(),
        swapchainRenderDestination->framebuffers[imageIndex],
        swapchain->swapchainExtent
    );
    renderPass.pass(commandBuffer.vkCommandBuffer, [&]() { _uiEngine.get().render(commandBuffer.vkCommandBuffer); });
}

bool Renderer::_drawScene(
    CommandBuffer& commandBuffer, DrawableScene* scene, uint32_t imageIndex, uint32_t roverCameraImageIndex
) {
    // if (!_renderPassDescriptors.contains("main") || !_renderPassDescriptors.contains("swapchain") ||
    //     !_renderDestinations.contains("main")) {
    //     _logger.error("Main render pass descriptor or main render destination not found!");
    //     return false;
    // }
    // auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    // auto& mainRenderDestination = _renderDestinations.at("main");
    // auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");
    auto pipelineLayout = _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    if (!pipelineLayout) {
        _logger.error("Failed to get base pipeline layout!");
        return false;
    }
    if (!_bindGlobalDescriptorSet(commandBuffer, pipelineLayout))
        return false;

    // RenderPassInfo renderPassInfo{
    //     .commandBuffer = &commandBuffer,
    //     .renderPassDescriptor = mainRenderPassDescriptor.get(),
    //     .pipelineLayout = pipelineLayout,
    //     .scene = scene,
    //     .imageIndex = imageIndex
    // };

    RenderPassNode::RunPassInfo runPassInfo{
        .commandBuffer = commandBuffer,
        .scene = scene,
        .imageIndex = imageIndex,
        .frameIndex = _currentFrame,
    };

    _renderPassGraph->run(runPassInfo);

    // RenderPass_V2::RunPassInfo runPassInfo{
    //     .commandBuffer = commandBuffer, .scene = scene, .imageIndex = imageIndex, .frameIndex = _currentFrame
    // };
    // _shadowPass->runPass(runPassInfo);
    // renderPassInfo.renderPassDescriptor = mainRenderPassDescriptor.get();
    // _mainPass->runPass(runPassInfo);
    // if (scene->shouldDrawSecondaryWindow() && roverCameraImageIndex != UINT32_MAX &&
    //     _renderDestinations.contains("rover_camera_main")) {
    //     RenderPass_V2::RunPassInfo roverCameraRunPassInfo{
    //         .commandBuffer = commandBuffer,
    //         .scene = scene,
    //         .imageIndex = roverCameraImageIndex,
    //         .frameIndex = _currentFrame
    //     };
    //     _roverCameraMainPass->runPass(roverCameraRunPassInfo);
    //     _roverCameraFBPass->runPass(roverCameraRunPassInfo);
    // }
    // _setViewportAndScissor(commandBuffer, _getSwapchain(0)->swapchainExtent);
    // _jfaPass->runPass(runPassInfo);
    // renderPassInfo.renderPassDescriptor = swapchainRenderPassDescriptor.get();
    // renderPassInfo.imageIndex = imageIndex;
    // _finalBlendPass(renderPassInfo);
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
    // if (!_renderDestinations.contains("main")) {
    //     _logger.error("Main render destination not found!");
    //     return;
    // }
    // auto& mainRenderDestination = _renderDestinations.at("main");
    // _createRenderDestinations();
    // if (_shadowPass.has_value()) {
    //     _shadowPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = _getSwapchain(0)->swapchainExtent});
    // }
    // _jfaPass->update(mainRenderDestination);
    // _writeFinalBlendDescriptorSets();
}

// void Renderer::_finalBlendPass(RenderPassInfo& renderPassInfo) {
//     if (!_renderDestinations.contains("swapchain")) {
//         _logger.error("Swapchain render destination not found!");
//         return;
//     }
//     auto& swapchainRenderDestination = _renderDestinations.at("swapchain");
//     _setViewportAndScissor(*renderPassInfo.commandBuffer, _getSwapchain(0)->swapchainExtent);
//     RenderPass renderPass(
//         *_vkEngine.get().device,
//         "swapchain_render_pass",
//         renderPassInfo.renderPassDescriptor->getVkRenderPass(),
//         swapchainRenderDestination->framebuffers[renderPassInfo.imageIndex],
//         _getSwapchain(0)->swapchainExtent
//     );
//     renderPass.pass(renderPassInfo.commandBuffer->vkCommandBuffer, [&]() {
//         auto pipelineLayout =
//             _vkEngine.get().pipelineManager->getPipelineLayout(vax::vk::PipelineLayoutName::FINAL_BLEND);
//         if (!pipelineLayout) {
//             _logger.error("Failed to get final blend pipeline layout!");
//             return;
//         }
//         auto finalBlendDescriptorSetHandler =
//         _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
//             _currentFrame,
//             vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
//             vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND,
//             "final_blend",
//             false
//         );
//         auto maskDescriptorSetName = _jfaPass->outputDescriptorSetName();
//         auto maskDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
//             _currentFrame,
//             vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
//             vax::vk::DescriptorSetManager::SetLayoutName::SINGLE_STORAGE_IMAGE,
//             maskDescriptorSetName,
//             false
//         );
//         auto perFrameDescriptorSetHandler = _vkEngine.get().descriptorSetManager->createDefaultDescriptorSetHandler(
//             _currentFrame,
//             vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
//             vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
//             "per_frame",
//             false
//         );
//         if (!finalBlendDescriptorSetHandler.has_value() || !maskDescriptorSetHandler.has_value() ||
//             !perFrameDescriptorSetHandler.has_value()) {
//             _logger.error("Failed to get default descriptor set writer!");
//             return;
//         }
//         finalBlendDescriptorSetHandler->bind(renderPassInfo.commandBuffer->vkCommandBuffer, pipelineLayout, 0);
//         maskDescriptorSetHandler->bind(renderPassInfo.commandBuffer->vkCommandBuffer, pipelineLayout, 1);
//         uint32_t offset = 0;
//         perFrameDescriptorSetHandler->bind(
//             renderPassInfo.commandBuffer->vkCommandBuffer,
//             pipelineLayout,
//             2,
//             VK_PIPELINE_BIND_POINT_GRAPHICS,
//             1,
//             &offset
//         );
//         auto pipeline = _vkEngine.get().pipelineManager->getPipeline(vax::vk::PipelineName::FINAL_BLEND);
//         if (!pipeline)
//             return;
//         if (!renderPassInfo.commandBuffer->bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS))
//             return;

//         DrawContext drawContext{
//             .commandBuffer = renderPassInfo.commandBuffer->vkCommandBuffer,
//             .pipelineLayout = pipeline->vkPipelineLayout,
//             .currentFrame = _currentFrame,
//         };
//         renderPassInfo.scene->drawBackground(drawContext);

//         renderPassInfo.commandBuffer->vkCommandBuffer);
//     });
// }_uiEngine.get().render(