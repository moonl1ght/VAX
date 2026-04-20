#pragma once

#include "luna.h"
#include "vkUtils.h"
#include "renderPass.h"
#include "deletionQueue.h"
#include "device.h"
#include "queueManager.h"
#include "window.h"
#include "pipelineManager.h"
#include "swapchain.h"
#include "renderDestination.h"
#include "descriptorSetManager.h"
#include "commandManager.h"

namespace vax::vk {
    class Device;
    class QueueManager;
}

namespace vax::vk {
    class Engine final {
    public:
        static const int MAX_FRAMES_IN_FLIGHT = 2;

        explicit Engine(vax::vk::Window& window) : _window(window) {};

        const uint32_t vulkanApiVersion = VK_API_VERSION_1_3;

        const bool enableValidationLayers = true;
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        bool framebufferResized = false;

        vax::utils::DeletionQueue deletionQueue;

        VkDebugUtilsMessengerEXT debugMessenger;

        VkInstance instance = VK_NULL_HANDLE;

        std::unique_ptr<vax::vk::Device> device;
        VmaAllocator allocator = VK_NULL_HANDLE;

        std::unique_ptr<vax::vk::QueueManager> queueManager;
        std::unique_ptr<vax::vk::Swapchain> swapchain;
        std::unique_ptr<vax::vk::RenderPass> renderPass;
        std::unique_ptr<vax::vk::RenderDestination> renderDestination;
        std::unique_ptr<vax::vk::DescriptorSetManager> descriptorSetManager;
        std::unique_ptr<vax::vk::PipelineManager> pipelineManager;
        std::unique_ptr<vax::vk::CommandManager> commandManager;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        bool setup();
        void cleanup();
        void resize();

    private:
        Logger _logger = Logger("Engine");

        std::reference_wrapper<vax::vk::Window> _window;

        bool setupDebugMessenger();
        bool createSyncObjects();
        VkResult createAllocator();
    };
}