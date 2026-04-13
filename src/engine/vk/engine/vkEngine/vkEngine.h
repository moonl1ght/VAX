#pragma once

#include "luna.h"
#include "vkUtils.h"
#include "RenderPassManager.hpp"
#include "deletionQueue.h"
#include "device.h"
#include "queueManager.h"
#include "window.h"

class RenderingDestination;
class DescriptorSetManager;
namespace vax {
    class PipelineManager;
}
namespace vax::vk {
    class Device;
    class QueueManager;
    class SwapchainManager;
}

namespace vax {
    class VkEngine final {
    public:
        VkEngine(vax::vk::Window& window) : _window(window) {};

        std::reference_wrapper<vax::vk::Window> _window;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        const uint32_t vulkanApiVersion = VK_API_VERSION_1_3;

        const bool enableValidationLayers = true;
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        bool framebufferResized = false;

        DeletionQueue deletionQueue;

        VkDebugUtilsMessengerEXT debugMessenger;

        VkInstance instance = VK_NULL_HANDLE;

        std::unique_ptr<vax::vk::Device> device;
        VmaAllocator allocator;

        std::unique_ptr<vax::vk::QueueManager> queueManager;
        std::unique_ptr<vax::vk::SwapchainManager> swapchainManager;

        RenderPassManager* renderPassManager = nullptr;
        RenderingDestination* renderingDestination = nullptr;
        DescriptorSetManager* descriptorSetManager = nullptr;
        vax::PipelineManager* pipelineManager = nullptr;

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
        Logger _logger = Logger("VkEngine");

        bool setupDebugMessenger();
        bool createCommandPool();
        bool createCommandBuffer();
        bool createSyncObjects();
        VkResult createAllocator();
    };
}