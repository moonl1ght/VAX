#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void App::run() {
    setup();
    mesh = Primitives2D::createPlane();
    mesh->loadBuffers(_vkStack);
    texture = TextureLoader::loadTexture(_vkStack, RES_PATH("images/texture.jpg"));

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < _vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers[i] = new Buffer(
            _vkStack,
            nullptr,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkMapMemory(_vkStack->device->vkDevice, uniformBuffers[i]->vkBufferMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
        // uniformBuffers[i]->fill(uniformBuffersMapped[i]);
    }

    // createDescriptorSets();


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

    delete mesh;
    mesh = nullptr;

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

// void App::createDescriptorSets() {
//     // std::cout << "createDescriptorSets 1" << std::endl;
//     std::vector<VkDescriptorSetLayout> layouts(_vkStack->MAX_FRAMES_IN_FLIGHT, _vkStack->descriptorSetLayout);
//     VkDescriptorSetAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.descriptorPool = _vkStack->descriptorPool;
//     allocInfo.descriptorSetCount = static_cast<uint32_t>(_vkStack->MAX_FRAMES_IN_FLIGHT);
//     allocInfo.pSetLayouts = layouts.data();

//     descriptorSets.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
//     // std::cout << "createDescriptorSets 2" << std::endl;
//     if (vkAllocateDescriptorSets(_vkStack->device->vkDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//         // std::cout << "createDescriptorSets error" << std::endl;
//         throw std::runtime_error("failed to allocate descriptor sets!");
//     }

//     // std::cout << "createDescriptorSets 3" << std::endl;
//     for (size_t i = 0; i < _vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
//         // std::cout << "createDescriptorSets loop" << std::endl;
//         VkDescriptorBufferInfo bufferInfo{};
//         // std::cout << "createDescriptorSets loop 1" << std::endl;
//         bufferInfo.buffer = uniformBuffers[i]->vkBuffer;
//         // std::cout << "createDescriptorSets loop 2" << std::endl;
//         bufferInfo.offset = 0;
//         bufferInfo.range = sizeof(UniformBufferObject);

//         VkDescriptorImageInfo imageInfo{};
//         imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//         imageInfo.imageView = texture->textureImageView;
//         imageInfo.sampler = texture->sampler->vkSampler;

//         std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

//         descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[0].dstSet = descriptorSets[i];
//         descriptorWrites[0].dstBinding = 0;
//         descriptorWrites[0].dstArrayElement = 0;
//         descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         descriptorWrites[0].descriptorCount = 1;
//         descriptorWrites[0].pBufferInfo = &bufferInfo;

//         descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         descriptorWrites[1].dstSet = descriptorSets[i];
//         descriptorWrites[1].dstBinding = 1;
//         descriptorWrites[1].dstArrayElement = 0;
//         descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//         descriptorWrites[1].descriptorCount = 1;
//         descriptorWrites[1].pImageInfo = &imageInfo;

//         vkUpdateDescriptorSets(
//             _vkStack->device->vkDevice,
//             static_cast<uint32_t>(descriptorWrites.size()),
//             descriptorWrites.data(),
//             0,
//             nullptr
//         );
//     }
//     // std::cout << "createDescriptorSets end" << std::endl;
// }

void App::updateUniformBuffer(uint32_t currentImage) {
    // std::cout << "--->>> updateUniformBuffer" << std::endl;
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), _vkStack->swapChainExtent.width / (float)_vkStack->swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
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

uint32_t currentFrame = 0;

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    // Logger::getInstance().debugPrint("recordCommandBuffer");
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
    VkBuffer vertexBuffers[] = { mesh->vertexBuffer.vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT16);

    // vkCmdDraw(commandBuffer, static_cast<uint32_t>(mesh->vertices.size()), 1, 0, 0);
    auto descriptorSet = _descriptorSetManager->getGlobalDescriptorSet(currentFrame, uniformBuffers[currentFrame], texture).value();
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineManager->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr
    );
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);

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

    updateUniformBuffer(currentFrame);

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