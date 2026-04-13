#include "Renderer.hpp"
#include "renderDestination.h"
#include "ImageUtils.hpp"
#include "pipeline.h"
#include "DescriptorSetManager.hpp"

void Renderer::prepare() {
    // Logger::getInstance().log("Preparing renderer...");
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
        _vkEngine->swapchain->swapchain,
        UINT64_MAX,
        _vkEngine->imageAvailableSemaphores[_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vkEngine->resize();
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

    if (
        vkQueueSubmit(_vkEngine->queueManager->graphicsQueue, 1, &submitInfo, _vkEngine->inFlightFences[_currentFrame]) != VK_SUCCESS
        ) {
        Logger::getInstance().error("failed to submit draw command buffer!");
        return false;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vkEngine->swapchain->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_vkEngine->queueManager->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _vkEngine->framebufferResized) {
        _vkEngine->framebufferResized = false;
        _vkEngine->resize();
        return false;
    }
    else if (result != VK_SUCCESS) {
        Logger::getInstance().error("failed to present swap chain image!");
        return false;
    }

    _currentFrame = (_currentFrame + 1) % _vkEngine->MAX_FRAMES_IN_FLIGHT;
    return true;
}

void Renderer::drawBackground(VkCommandBuffer commandBuffer) {
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(_currentFrame / 120.f));
    clearValue = { { 1.0f, 1.0f, 1.0f, 1.0f } };

    VkImageSubresourceRange clearRange = vax::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(
        commandBuffer,
        _vkEngine->renderDestination->drawImage->textureImage,
        VK_IMAGE_LAYOUT_GENERAL,
        &clearValue,
        1,
        &clearRange
    );
    auto& backgroundPipeline = _vkEngine->pipelineManager->getBackgroundPipeline();
    std::cout << "Background pipeline: " << backgroundPipeline.vkPipeline << std::endl;
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, backgroundPipeline.vkPipeline);

    auto drawBackgroundDescriptorSet = _vkEngine->descriptorSetManager->getDrawBackgroundDescriptorSet(_currentFrame);
    if (!drawBackgroundDescriptorSet.has_value()) {
        LOG_ERROR("Failed to get draw background descriptor set!");
        return;
    }
    DescriptorWriter descriptorWriter;
    descriptorWriter.writeStorageImage(_vkEngine->renderDestination->drawImage->textureImageView, 0);
    descriptorWriter.updateSet(_vkEngine->device->vkDevice, drawBackgroundDescriptorSet.value());
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        backgroundPipeline.vkPipelineLayout,
        0,
        1,
        &drawBackgroundDescriptorSet.value(),
        0,
        nullptr
    );

    vkCmdDispatch(commandBuffer, std::ceil(_vkEngine->renderDestination->drawImage->size.width / 16.0), std::ceil(_vkEngine->renderDestination->drawImage->size.height / 16.0), 1);
}

bool Renderer::recordCommandBuffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene, float deltaTime
) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (!VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))) {
        LOG_ERROR("Failed to begin recording command buffer!");
        return false;
    }

    vax::transitionImage(
        commandBuffer,
        _vkEngine->renderDestination->drawImage->textureImage,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL
    );

    drawBackground(commandBuffer);

    vax::transitionImage(
        commandBuffer,
        _vkEngine->renderDestination->drawImage->textureImage,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    );
    vax::transitionImage(
        commandBuffer,
        _vkEngine->swapchain->swapchainImages[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    vax::copyImageToImage(
        commandBuffer,
        _vkEngine->renderDestination->drawImage->textureImage,
        _vkEngine->swapchain->swapchainImages[imageIndex],
        _vkEngine->renderDestination->drawImage->size.toExtent2D(),
        _vkEngine->swapchain->swapchainExtent
    );
    // std::cout << "Image size: " << _vkEngine->renderingDestination->drawImage->size.toExtent2D().width << "x" << _vkEngine->renderingDestination->drawImage->size.toExtent2D().height << std::endl;
    // std::cout << "Swapchain size: " << _vkEngine->swapchainManager->swapchainExtent.width << "x" << _vkEngine->swapchainManager->swapchainExtent.height << std::endl;

    vax::transitionImage(
        commandBuffer,
        _vkEngine->swapchain->swapchainImages[imageIndex],
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    // VkRenderPassBeginInfo renderPassInfo{};
    // renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // renderPassInfo.renderPass = _vkEngine->renderPassManager->getRenderPass();
    // renderPassInfo.framebuffer = _vkEngine->renderingDestination->swapchainFramebuffers[imageIndex];
    // renderPassInfo.renderArea.offset = { 0, 0 };
    // renderPassInfo.renderArea.extent = _vkEngine->swapchainManager->swapchainExtent;

    // std::array<VkClearValue, 2> clearValues{};
    // clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    // clearValues[1].depthStencil = { 1.0f, 0 };

    // renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    // renderPassInfo.pClearValues = clearValues.data();

    // vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _vkEngine->pipelineManager->getPipeline());

    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float)_vkEngine->swapchainManager->swapchainExtent.width;
    // viewport.height = (float)_vkEngine->swapchainManager->swapchainExtent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    // vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // VkRect2D scissor{};
    // scissor.offset = { 0, 0 };
    // scissor.extent = _vkEngine->swapchainManager->swapchainExtent;
    // vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // memcpy(_sceneUniformBuffersMapped[_currentFrame], &scene->getUBO(), sizeof(scene->getUBO()));

    // auto descriptorSet = _vkEngine->descriptorSetManager->getGlobalDescriptorSet(
    //     _currentFrame, _sceneUniformBuffers[_currentFrame], scene->texture
    // );
    // if (!descriptorSet.has_value()) {
    //     Logger::getInstance().error("Failed to get global descriptor set!");
    //     return false;
    // }
    // std::vector<VkDescriptorSet> descriptorSets = { descriptorSet.value() };
    // vkCmdBindDescriptorSets(
    //     commandBuffer,
    //     VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     _vkEngine->pipelineManager->getPipelineLayout(),
    //     0,
    //     static_cast<uint32_t>(descriptorSets.size()),
    //     descriptorSets.data(),
    //     0,
    //     nullptr
    // );

    // for (auto& drawableModel : scene->getDrawableModels()) {
    //     drawableModel->draw(_vkEngine, commandBuffer, _vkEngine->pipelineManager, deltaTime);
    // }

    // vkCmdEndRenderPass(commandBuffer);

    if (!VK_CHECK(vkEndCommandBuffer(commandBuffer))) {
        LOG_ERROR("Failed to end command buffer!");
        return false;
    }
    return true;
}

