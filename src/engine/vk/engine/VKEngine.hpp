#pragma once

#include "luna.h"
#include "VKUtils.hpp"
#include "Device.hpp"
#include "RenderPassManager.hpp"
#include "DeletionQueue.hpp"

class SwapchainManager;
class RenderingDestination;
class DescriptorSetManager;
class PipelineManager;

class VKEngine final {
public:
    VKEngine(SDL_Window* window) : window(window) {};

    const int MAX_FRAMES_IN_FLIGHT = 2;
    const uint32_t vulkanApiVersion = VK_API_VERSION_1_2;

    const bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool framebufferResized = false;

    SDL_Window* window;
    VkInstance instance;
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
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            Logger::getInstance().log("validation layer: ", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            Logger::getInstance().error("validation layer: ", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            Logger::getInstance().error("validation layer: ", pCallbackData->pMessage);
            break;
        default:
            Logger::getInstance().warning("validation layer: ", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    bool createInstance();
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    bool setupDebugMessenger();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    VkResult createAllocator();
};