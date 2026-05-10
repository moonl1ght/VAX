#include "renderer.h"
#include "renderDestination.h"
#include "imageUtils.h"
#include "pipeline.h"
#include "descriptorSetManager.h"
#include "vkEngine.h"
#include "imgui_impl_vulkan.h"
#include "rendererPass.h"

using namespace vax::renderer;
using namespace vax;

void Renderer::prepare() {
}

bool Renderer::render(DrawableScene* scene, float deltaTime) {
    ZoneScopedN("Renderer::render");
    scene->prepareForDraw(renderer::RenderCallContext{
        .currentFrame = _currentFrame
    });

    vkWaitForFences(
        _vkEngine.get().device->vkDevice,
        1,
        &_vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame],
        VK_TRUE, UINT64_MAX
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
        scene->resize();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        _logger.error("Failed to acquire swap chain image!");
        return false;
    }

    vkResetFences(
        _vkEngine.get().device->vkDevice,
        1,
        &_vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame]
    );

    vkResetCommandBuffer(_vkEngine.get().commandManager->commandBuffers[_currentFrame], 0);
    auto updateResult = _updateCommandBuffer(
        _vkEngine.get().commandManager->commandBuffers[_currentFrame],
        imageIndex,
        scene
    );
    if (!updateResult) {
        _logger.error("Failed to update command buffer!");
        return false;
    }

    VkSemaphore waitSemaphores[] = {
        _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame]
    };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = {
        _vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores()[_currentFrame]
    };
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

    if (!VK_CHECK(
        vkQueueSubmit(
            _vkEngine.get().queueManager->graphicsQueue,
            1,
            &submitInfo,
            _vkEngine.get().syncObjectsManager->getInFlightFences()[_currentFrame]
        ))) {
        _logger.error("failed to submit draw command buffer!");
        return false;
    }

    VkSwapchainKHR swapChains[] = { _vkEngine.get().swapchain->swapchain };
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
        scene->resize();
        return false;
    }
    else if (result != VK_SUCCESS) {
        _logger.error("failed to present swap chain image!");
        return false;
    }

    _currentFrame = (_currentFrame + 1) % vax::MAX_FRAMES_IN_FLIGHT;
    return true;
}

bool Renderer::_updateCommandBuffer(
    VkCommandBuffer commandBuffer,
    uint32_t imageIndex,
    vax::DrawableScene* scene
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

    if (!_updateGlobalDescriptorSet(commandBuffer, scene)) {
        _logger.error("Failed to update global descriptor set!");
        return false;
    }

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = _vkEngine.get().renderPass->getVkRenderPass(),
        .framebuffer = _vkEngine.get().renderDestination->swapchainFramebuffers[imageIndex],
        .renderArea = {
            .offset = { 0, 0 },
            .extent = _vkEngine.get().swapchain->swapchainExtent
        },
        .clearValueCount = 2,
        .pClearValues = clearValues.data()
    };
    RendererPass renderPass(renderPassInfo);
    renderPass.pass(commandBuffer, [&]() {
        vkCmdBindPipeline(
            commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vkEngine.get().pipelineManager->getPipeline()
        );

        auto frameDescriptorSetWriter = _vkEngine.get().descriptorSetManager->getDescriptorSetWriter(
            _currentFrame, vax::vk::DescriptorSetLayout::SetType::PER_FRAME
        );

        if (!frameDescriptorSetWriter.has_value()) {
            _logger.error("Failed to get default descriptor set writer!");
            return;
        }
        scene->writeFrameDescriptorSet(*frameDescriptorSetWriter);
        frameDescriptorSetWriter->update();
        VkDescriptorSet frameDescriptorSet = frameDescriptorSetWriter->getDescriptorSet();
        std::vector<VkDescriptorSet> descriptorSets = { frameDescriptorSet };
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _vkEngine.get().pipelineManager->getPipelineLayout(),
            SetIndices::PER_FRAME_SET_INDEX,
            static_cast<uint32_t>(descriptorSets.size()),
            descriptorSets.data(),
            0,
            nullptr
        );

        scene->draw(commandBuffer);

        _uiLayer.get().render(commandBuffer);
    });

    if (!VK_CHECK(vkEndCommandBuffer(commandBuffer))) {
        _logger.error("Failed to end command buffer!");
        return false;
    }
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

    VkRect2D scissor{
        .offset = { 0, 0 },
        .extent = _vkEngine.get().swapchain->swapchainExtent
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

bool Renderer::_updateGlobalDescriptorSet(VkCommandBuffer commandBuffer, vax::DrawableScene* scene) {
    auto globalDescriptorSetWriter = _vkEngine.get().descriptorSetManager->getDescriptorSetWriter(
        _currentFrame, vax::vk::DescriptorSetLayout::SetType::GLOBAL
    );
    if (!globalDescriptorSetWriter.has_value()) {
        return false;
    }
    scene->writeGlobalDescriptorSet(*globalDescriptorSetWriter);
    globalDescriptorSetWriter->update();
    VkDescriptorSet globalDescriptorSet = globalDescriptorSetWriter->getDescriptorSet();
    std::vector<VkDescriptorSet> descriptorSets = { globalDescriptorSet };
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _vkEngine.get().pipelineManager->getPipelineLayout(),
        SetIndices::GLOBAL_SET_INDEX,
        static_cast<uint32_t>(descriptorSets.size()),
        descriptorSets.data(),
        0,
        nullptr
    );
    return true;
}