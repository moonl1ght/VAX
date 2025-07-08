#include "App.hpp"

uint32_t currentFrame = 0;

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void App::run() {
    setup();
    model = Primitives2D::createPlane();
    model->mesh->loadBuffers(_vkStack);
    texture = TextureLoader::loadTexture(_vkStack, RES_PATH("images/texture.jpg"));

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
    // objectUniformBuffers.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
    // objectUniformBuffersMapped.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < _vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers[i] = new Buffer(
            _vkStack,
            nullptr,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkMapMemory(_vkStack->device->vkDevice, uniformBuffers[i]->vkBufferMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);

        // VkDeviceSize bufferSize1 = sizeof(ObjectUniforms);

        // objectUniformBuffers[i] = new Buffer(
        //     _vkStack,
        //     nullptr,
        //     bufferSize1,
        //     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        // );
        // std::cout << "Object uniform buffer size: " << (objectUniformBuffers[i]->vkBufferMemory == VK_NULL_HANDLE) << std::endl;
        // objectUniformBuffers.push_back(buffer);

        // vkMapMemory(_vkStack->device->vkDevice, objectUniformBuffers[i]->vkBufferMemory, 0, bufferSize1, 0, &objectUniformBuffersMapped[i]);

        // uniformBuffers[i]->fill(uniformBuffersMapped[i]);
    }


    std::cout << "--->>> main loop" << std::endl;
    mainLoop();
    cleanup();
}

void App::setup() {
    initWindow();
    _vkStack = new VKStack(_window);
    _vkStack->setup();
    _descriptorSetManager = new DescriptorSetManager(_vkStack);
    _descriptorSetManager->initialize();
    _pipelineManager = new PipelineManager(_vkStack, _descriptorSetManager);
    _pipelineManager->initialize();
}

void App::initWindow() {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void App::cleanup() {
    Logger::getInstance().log("Cleaning up...");

    delete model;
    model = nullptr;

    delete texture;
    texture = nullptr;

    for (size_t i = 0; i < _vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        delete uniformBuffers[i];
        uniformBuffers[i] = nullptr;
    }

    delete _descriptorSetManager;
    _descriptorSetManager = nullptr;
    delete _pipelineManager;
    _pipelineManager = nullptr;

    _vkStack->cleanup();

    if (_window != nullptr) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    glfwTerminate();
    delete _vkStack;
    _vkStack = nullptr;
}

void App::updateUniformBuffer(VkCommandBuffer commandBuffer, uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), _vkStack->swapChainExtent.width / (float)_vkStack->swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    DrawPushConstants drawPushConstants{};
    drawPushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    vkCmdPushConstants(
        commandBuffer,
        _pipelineManager->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(DrawPushConstants),
        &drawPushConstants
    );

    // ObjectUniforms objectUniforms{};
    // objectUniforms.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));

    // VkDeviceSize bufferSize = sizeof(ObjectUniforms);

    // if (buffer == nullptr) {
    //     buffer = new Buffer(
    //         _vkStack,
    //         nullptr,
    //         bufferSize,
    //         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    //         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    //     );
    // }
    // buffer->fill(&objectUniforms);

    // objectUniformBuffers[currentImage]->fill(&objectUniforms);

    // std::cout << "Updating object uniform buffer" << std::endl;
    // memcpy(objectUniformBuffersMapped[currentImage], &objectUniforms, sizeof(objectUniforms));

    // auto descriptorSet = _descriptorSetManager->getObjectDescriptorSet(currentImage);
    // DescriptorWriter writer = DescriptorWriter();
    // // std::cout << "Object uniform buffer size: " << (objectUniformBuffers[currentImage]->vkBufferMemory) << std::endl;
    // // VkMemoryRequirements memRequirements;
    // // vkGetBufferMemoryRequirements(_vkStack->device->vkDevice, objectUniformBuffers[currentImage]->vkBuffer, &memRequirements);
    // // std::cout << "Object uniform buffer size memRequirements: " << memRequirements.size << std::endl;
    // writer.writeBuffer(buffer, 0);
    // writer.updateSet(_vkStack->device->vkDevice, descriptorSet.value());

    // VkDescriptorBufferInfo bufferInfo{
    //     .buffer = buffer->vkBuffer,
    //     .offset = 0,
    //     .range = buffer->size
    // };

    // // VkMemoryRequirements memRequirements;
    // // vkGetBufferMemoryRequirements(_vkStack->device->vkDevice, buffer->vkBuffer, &memRequirements);
    // // std::cout << "Object uniform buffer size memRequirements: " << memRequirements.size << std::endl;

    // std::vector<VkWriteDescriptorSet> writes;
    // VkWriteDescriptorSet write{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = descriptorSet.value(),
    //     .dstBinding = 0,
    //     .dstArrayElement = 0,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .descriptorCount = 1,
    //     .pBufferInfo = &bufferInfo
    // };
    // writes.push_back(write);

    // vkUpdateDescriptorSets(_vkStack->device->vkDevice, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void App::mainLoop() {
    if (_window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(_vkStack->device->vkDevice);
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _vkStack->renderPass;
    renderPassInfo.framebuffer = _vkStack->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = _vkStack->swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineManager->getPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_vkStack->swapChainExtent.width;
    viewport.height = (float)_vkStack->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = _vkStack->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // std::cout << "--->> draw mesh" << std::endl;
    VkBuffer vertexBuffers[] = { model->mesh->vertexBuffer.vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, model->mesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT16);

    updateUniformBuffer(_vkStack->commandBuffers[currentFrame], currentFrame);
    // vkCmdDraw(commandBuffer, static_cast<uint32_t>(mesh->vertices.size()), 1, 0, 0);
    auto descriptorSet = _descriptorSetManager->getGlobalDescriptorSet(currentFrame, uniformBuffers[currentFrame], texture).value();
    // updateUniformBuffer(currentFrame);
    // auto descriptorSet1 = _descriptorSetManager->getObjectDescriptorSet(currentFrame);
    std::vector<VkDescriptorSet> descriptorSets = { descriptorSet };
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
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(model->mesh->indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::drawFrame() {
    // std::cout << "Drawing frame..." << std::endl;
    vkWaitForFences(_vkStack->device->vkDevice, 1, &_vkStack->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        _vkStack->device->vkDevice, _vkStack->swapChain, UINT64_MAX, _vkStack->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vkStack->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(_vkStack->device->vkDevice, 1, &_vkStack->inFlightFences[currentFrame]);

    vkResetCommandBuffer(_vkStack->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(_vkStack->commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vkStack->imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vkStack->commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { _vkStack->renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_vkStack->graphicsQueue, 1, &submitInfo, _vkStack->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vkStack->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_vkStack->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        _vkStack->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % _vkStack->MAX_FRAMES_IN_FLIGHT;
}