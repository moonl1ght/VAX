#include "renderer.h"
#include "renderDestination.h"
#include "imageUtils.h"
#include "pipeline.h"
#include "descriptorSetManager.h"
#include "vkEngine.h"
#include "imgui_impl_vulkan.h"

using namespace vax;

void Renderer::prepare() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.reserve(vk::Engine::MAX_FRAMES_IN_FLIGHT);
    _sceneUniformBuffersMapped.resize(vk::Engine::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < vk::Engine::MAX_FRAMES_IN_FLIGHT; i++) {
        auto& bufferManager = _vkEngine.get().resourceManager->bufferManager();
        auto allocation = bufferManager.allocateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value();
        _sceneUniformBuffers.push_back(allocation.second);
        vkMapMemory(
            _vkEngine.get().device->vkDevice,
            _sceneUniformBuffers[i]->getVkBufferMemory(),
            0,
            bufferSize,
            0,
            &_sceneUniformBuffersMapped[i]
        );

        // _sceneUniformBuffers[i].bind(_sceneUniformBuffersMapped[i]);
    }
}

bool Renderer::render(Scene* scene, float deltaTime, ImDrawData* imguiDrawData) {
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
    if (!recordCommandBuffer(_vkEngine.get().commandManager->commandBuffers[_currentFrame], imageIndex, scene, deltaTime, imguiDrawData)) {
        _logger.error("Failed to record command buffer!");
        return false;
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vkEngine.get().syncObjectsManager->getImageAvailableSemaphores()[_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkEngine.get().commandManager->commandBuffers[_currentFrame];

    VkSemaphore signalSemaphores[] = { _vkEngine.get().syncObjectsManager->getRenderFinishedSemaphores()[_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

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

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vkEngine.get().swapchain->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_vkEngine.get().queueManager->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _vkEngine.get().framebufferResized) {
        _vkEngine.get().framebufferResized = false;
        _vkEngine.get().resize();
        return false;
    }
    else if (result != VK_SUCCESS) {
        _logger.error("failed to present swap chain image!");
        return false;
    }

    _currentFrame = (_currentFrame + 1) % vk::Engine::MAX_FRAMES_IN_FLIGHT;
    return true;
}

bool Renderer::recordCommandBuffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene, float deltaTime, ImDrawData* imguiDrawData
) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (!VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))) {
        _logger.error("Failed to begin recording command buffer!");
        return false;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _vkEngine.get().renderPass->getVkRenderPass();
    renderPassInfo.framebuffer = _vkEngine.get().renderDestination->swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = _vkEngine.get().swapchain->swapchainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vkEngine.get().pipelineManager->getPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_vkEngine.get().swapchain->swapchainExtent.width;
    viewport.height = (float)_vkEngine.get().swapchain->swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = _vkEngine.get().swapchain->swapchainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    memcpy(_sceneUniformBuffersMapped[_currentFrame], &scene->getUBO(), sizeof(scene->getUBO()));

    auto descriptorSetWriter = _vkEngine.get().descriptorSetManager->getDefaultDescriptorSetWriter(
        _currentFrame, vax::vk::DescriptorSetLayout::DefaultType::BASE
    );
    if (descriptorSetWriter.has_value()) {
        descriptorSetWriter.value().writeBuffer(_sceneUniformBuffers[_currentFrame], 0);
        // descriptorSet.writeTexture(scene->texture, 1);
        descriptorSetWriter.value().update();
    } else {
        _logger.error("Failed to get default descriptor set writer!");
        return false;
    }
    VkDescriptorSet descriptorSet = descriptorSetWriter.value().getDescriptorSet();
    std::vector<VkDescriptorSet> descriptorSets = { descriptorSet };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _vkEngine.get().pipelineManager->getPipelineLayout(),
        0,
        static_cast<uint32_t>(descriptorSets.size()),
        descriptorSets.data(),
        0,
        nullptr
    );

    for (auto& drawableModel : scene->getDrawableModels()) {
        drawableModel.draw(&_vkEngine.get(), commandBuffer, *(_vkEngine.get().pipelineManager), deltaTime);
    }

    ImGui_ImplVulkan_RenderDrawData(imguiDrawData, commandBuffer);
    vkCmdEndRenderPass(commandBuffer);

    if (!VK_CHECK(vkEndCommandBuffer(commandBuffer))) {
        _logger.error("Failed to end command buffer!");
        return false;
    }
    return true;
}

