#include "Renderer.hpp"

void Renderer::prepare() {
    Logger::getInstance().log("Preparing renderer...");
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    _sceneUniformBuffers.resize(_vkEngine->MAX_FRAMES_IN_FLIGHT);
    _sceneUniformBuffersMapped.resize(_vkEngine->MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < _vkEngine->MAX_FRAMES_IN_FLIGHT; i++) {
        _sceneUniformBuffers[i] = new Buffer(
            _vkEngine,
            nullptr,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkMapMemory(
            _vkEngine->device->vkDevice,
            _sceneUniformBuffers[i]->vkBufferMemory,
            0,
            bufferSize,
            0,
            &_sceneUniformBuffersMapped[i]
        );
    }
}

bool Renderer::render(Scene* scene, float deltaTime) {
    vkWaitForFences(_vkEngine->device->vkDevice, 1, &_vkEngine->inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        _vkEngine->device->vkDevice,
        _vkEngine->swapChain,
        UINT64_MAX,
        _vkEngine->imageAvailableSemaphores[_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vkEngine->recreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        Logger::getInstance().error("Failed to acquire swap chain image!");
        return false;
    }

    vkResetFences(_vkEngine->device->vkDevice, 1, &_vkEngine->inFlightFences[_currentFrame]);

    vkResetCommandBuffer(_vkEngine->commandBuffers[_currentFrame], 0);
    if (!recordCommandBuffer(_vkEngine->commandBuffers[_currentFrame], imageIndex, scene, deltaTime)) {
        Logger::getInstance().error("Failed to record command buffer!");
        return false;
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vkEngine->imageAvailableSemaphores[_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkEngine->commandBuffers[_currentFrame];

    VkSemaphore signalSemaphores[] = { _vkEngine->renderFinishedSemaphores[_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_vkEngine->graphicsQueue, 1, &submitInfo, _vkEngine->inFlightFences[_currentFrame]) != VK_SUCCESS) {
        Logger::getInstance().error("failed to submit draw command buffer!");
        return false;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vkEngine->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_vkEngine->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _vkEngine->framebufferResized) {
        _vkEngine->framebufferResized = false;
        _vkEngine->recreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS) {
        Logger::getInstance().error("failed to present swap chain image!");
        return false;
    }

    _currentFrame = (_currentFrame + 1) % _vkEngine->MAX_FRAMES_IN_FLIGHT;
    return true;
}

bool Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene, float deltaTime) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        Logger::getInstance().error("failed to begin recording command buffer!");
        return false;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _vkEngine->renderPass;
    renderPassInfo.framebuffer = _vkEngine->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = _vkEngine->swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineManager->getPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_vkEngine->swapChainExtent.width;
    viewport.height = (float)_vkEngine->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = _vkEngine->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    memcpy(_sceneUniformBuffersMapped[_currentFrame], &scene->getUBO(), sizeof(scene->getUBO()));

    auto descriptorSet = _descriptorSetManager->getGlobalDescriptorSet(
        _currentFrame, _sceneUniformBuffers[_currentFrame], scene->texture
    );
    if (!descriptorSet.has_value()) {
        Logger::getInstance().error("Failed to get global descriptor set!");
        return false;
    }
    std::vector<VkDescriptorSet> descriptorSets = { descriptorSet.value() };
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _pipelineManager->getPipelineLayout(),
        0,
        static_cast<uint32_t>(descriptorSets.size()),
        descriptorSets.data(),
        0,
        nullptr
    );

    for (auto& drawableModel : scene->getDrawableModels()) {
        drawableModel->draw(_vkEngine, commandBuffer, _pipelineManager, deltaTime);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        Logger::getInstance().error("failed to record command buffer!");
        return false;
    }
    return true;
}

