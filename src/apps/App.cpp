#include "App.hpp"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void App::run() {
    // std::cout << "--->>> will run" << std::endl;
    initWindow();
    vkStack = new VKStack(window);
    // std::cout << "--->>> setup" << std::endl;
    vkStack->setup();
    // std::cout << "--->>> createDescriptorSets" << std::endl;
    // std::cout << "--->>> did createDescriptorSets" << std::endl;
    mesh = Primitives2D::createPlane();
    mesh->prepareForRender(vkStack);
    texture = TextureLoader::loadTexture(vkStack, RES_PATH("images/texture.jpg"));

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(vkStack->MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(vkStack->MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers[i] = new Buffer(
            vkStack,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkMapMemory(vkStack->device->vkDevice, uniformBuffers[i]->vkBufferMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
        // uniformBuffers[i]->fill(uniformBuffersMapped[i]);
    }

    createDescriptorSets();

    std::cout << "--->>> main loop" << std::endl;
    mainLoop();
    cleanup();
}

void App::initWindow() {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void App::cleanup() {
    std::cout << "Cleaning up..." << std::endl;

    mesh->cleanup(vkStack);
    delete mesh;
    mesh = nullptr;

    delete texture;
    texture = nullptr;

    for (size_t i = 0; i < vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        delete uniformBuffers[i];
        uniformBuffers[i] = nullptr;
    }

    vkStack->cleanup();

    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
    delete vkStack;
    vkStack = nullptr;
}

void App::createDescriptorSets() {
    // std::cout << "createDescriptorSets 1" << std::endl;
    std::vector<VkDescriptorSetLayout> layouts(vkStack->MAX_FRAMES_IN_FLIGHT, vkStack->descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vkStack->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(vkStack->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(vkStack->MAX_FRAMES_IN_FLIGHT);
    // std::cout << "createDescriptorSets 2" << std::endl;
    if (vkAllocateDescriptorSets(vkStack->device->vkDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        // std::cout << "createDescriptorSets error" << std::endl;
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // std::cout << "createDescriptorSets 3" << std::endl;
    for (size_t i = 0; i < vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        // std::cout << "createDescriptorSets loop" << std::endl;
        VkDescriptorBufferInfo bufferInfo{};
        // std::cout << "createDescriptorSets loop 1" << std::endl;
        bufferInfo.buffer = uniformBuffers[i]->vkBuffer;
        // std::cout << "createDescriptorSets loop 2" << std::endl;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->textureImageView;
        imageInfo.sampler = texture->sampler->vkSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(
            vkStack->device->vkDevice,
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr
        );
    }
    // std::cout << "createDescriptorSets end" << std::endl;
}

void App::updateUniformBuffer(uint32_t currentImage) {
    // std::cout << "--->>> updateUniformBuffer" << std::endl;
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 3, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), vkStack->swapChainExtent.width / (float)vkStack->swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void App::mainLoop() {
    if (window == nullptr) {
        throw std::runtime_error("Window not initialized");
    }
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(vkStack->device->vkDevice);
}

uint32_t currentFrame = 0;

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkStack->renderPass;
    renderPassInfo.framebuffer = vkStack->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = vkStack->swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkStack->graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vkStack->swapChainExtent.width;
    viewport.height = (float)vkStack->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = vkStack->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // std::cout << "--->> draw mesh" << std::endl;
    VkBuffer vertexBuffers[] = { mesh->vertexBuffer->vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer->vkBuffer, 0, VK_INDEX_TYPE_UINT16);

    // vkCmdDraw(commandBuffer, static_cast<uint32_t>(mesh->vertices.size()), 1, 0, 0);
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkStack->pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr
    );
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::drawFrame() {
    // std::cout << "Drawing frame..." << std::endl;
    vkWaitForFences(vkStack->device->vkDevice, 1, &vkStack->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        vkStack->device->vkDevice, vkStack->swapChain, UINT64_MAX, vkStack->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkStack->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);

    vkResetFences(vkStack->device->vkDevice, 1, &vkStack->inFlightFences[currentFrame]);

    vkResetCommandBuffer(vkStack->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(vkStack->commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { vkStack->imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkStack->commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { vkStack->renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vkStack->graphicsQueue, 1, &submitInfo, vkStack->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { vkStack->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vkStack->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        vkStack->recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % vkStack->MAX_FRAMES_IN_FLIGHT;
}