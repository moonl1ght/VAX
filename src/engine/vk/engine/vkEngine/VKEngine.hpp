#pragma once

#include "luna.h"
#include "VKUtils.hpp"
#include "device.h"
#include "RenderPassManager.hpp"
#include "deletionQueue.h"

class SwapchainManager;
class RenderingDestination;
class DescriptorSetManager;
class PipelineManager;

class VKEngine final {
public:
    VKEngine(SDL_Window* window) : window(window) {};

    const int MAX_FRAMES_IN_FLIGHT = 2;
    const uint32_t vulkanApiVersion = VK_API_VERSION_1_3;

    const bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool framebufferResized = false;

    SDL_Window* window;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    vax::Device* device;
    VmaAllocator allocator;
    DeletionQueue deletionQueue;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    SwapchainManager* swapchainManager = nullptr;
    RenderPassManager* renderPassManager = nullptr;
    RenderingDestination* renderingDestination = nullptr;
    DescriptorSetManager* descriptorSetManager = nullptr;
    PipelineManager* pipelineManager = nullptr;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    bool setup();
    void cleanup();

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    bool setupDebugMessenger();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    VkResult createAllocator();
};