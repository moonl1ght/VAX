#include "vkEngine.h"
#include "TextureLoader.hpp"
#include "texture.h"
#include "renderDestination.h"
#include "descriptorSetManager.h"
#include "vk_debug.h"
#include "vkInstanceBuilder.h"
#include "renderPassBuilder.h"
#include "renderDestinationBuilder.h"

using namespace vax::vk;

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator
) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void validationLayersDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator
) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

bool vax::vk::Engine::setup() {
    std::optional<VkInstance> instanceOptional = vax::VkInstanceBuilder(
        deletionQueue,
        enableValidationLayers,
        validationLayers,
        vulkanApiVersion
    ).build();
    if (instanceOptional.has_value()) {
        std::cout << "Instance created" << instanceOptional.value() << std::endl;
        instance = *instanceOptional;
    }
    else {
        return false;
    }
    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying instance...");
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    );

    if (!setupDebugMessenger()) return false;

    _window.get().createSurface(instance);
    deletionQueue.push_function(
        [&]() {
            _window.get().destroySurface();
        }
    );

    device = std::make_unique<Device>();
    if (!device->load(instance, _window.get().surface, enableValidationLayers)) {
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            device->destroy();
        }
    );

    queueManager = std::make_unique<QueueManager>();
    queueManager->setup(*device);

    _logger.info("Creating allocator...");
    if (!VK_CHECK(createAllocator())) {
        _logger.error("Failed to create allocator!");
        return false;
    }
    std::cout << "Allocator created: " << allocator << std::endl;

    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying allocator...");
            vmaDestroyAllocator(allocator);
        }
    );

    if (!createCommandPool()) return false;

    if (!createCommandBuffer()) return false;

    if (!createSyncObjects()) return false;

    swapchain = std::make_unique<Swapchain>(_window.get(), *device);
    if (!swapchain->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            swapchain->cleanup();
        }
    );

    std::optional<std::unique_ptr<RenderPass>> renderPassOptional = RenderPassBuilder(
        *device,
        *swapchain
    ).build();
    if (!renderPassOptional.has_value()) return false;
    renderPass = std::move(*renderPassOptional);

    auto renderDestinationOptional = RenderDestinationBuilder(
        *device,
        allocator,
        *swapchain,
        *renderPass
    ).build(this);
    if (!renderDestinationOptional.has_value()) return false;
    renderDestination = std::move(*renderDestinationOptional);

    descriptorSetManager = std::make_unique<DescriptorSetManager>(this);
    if (!descriptorSetManager->setup()) return false;

    pipelineManager = std::make_unique<PipelineManager>(*device, *descriptorSetManager);
    pipelineManager->setup(*renderPass);

    _logger.info("Engine setup complete!");
    return true;
}

void vax::vk::Engine::cleanup() {
    deletionQueue.flush();
    _logger.info("Engine cleanup complete!");
}

void vax::vk::Engine::resize() {
    _logger.info("Resizing engine...");
    int width = 0, height = 0;
    SDL_GetWindowSizeInPixels(_window.get().window, &width, &height);
    while (width == 0 || height == 0) {
        SDL_GetWindowSizeInPixels(_window.get().window, &width, &height);
        SDL_Delay(100);
    }
    vkDeviceWaitIdle(device->vkDevice);

    swapchain->recreate();
    auto renderDestinationOptional = RenderDestinationBuilder(
        *device,
        allocator,
        *swapchain,
        *renderPass
    ).build(this);
    if (!renderDestinationOptional.has_value()) return;
    renderDestination = std::move(*renderDestinationOptional);
}

bool vax::vk::Engine::setupDebugMessenger() {
    LOG_INFO("Setting up debug messenger...");
    if (!enableValidationLayers) return false;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    vax::populateDebugMessengerCreateInfo(createInfo);

    if (!VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))) {
        LOG_ERROR("Failed to set up debug messenger!");
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying debug messenger...");
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
    );

    return true;
}

bool vax::vk::Engine::createCommandPool() {
    LOG_INFO("Creating command pool...");
    utils::QueueFamilyIndices queueFamilyIndices = device->getQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (!VK_CHECK(vkCreateCommandPool(device->vkDevice, &poolInfo, nullptr, &commandPool))) {
        LOG_ERROR("Failed to create command pool!");
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying command pool...");
            vkDestroyCommandPool(device->vkDevice, commandPool, nullptr);
        }
    );

    return true;
}

bool vax::vk::Engine::createCommandBuffer() {
    LOG_INFO("Creating command buffer...");
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (!VK_CHECK(vkAllocateCommandBuffers(device->vkDevice, &allocInfo, commandBuffers.data()))) {
        LOG_ERROR("Failed to allocate command buffers!");
        return false;
    }

    return true;
}

bool vax::vk::Engine::createSyncObjects() {
    LOG_INFO("Creating synchronization objects...");
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (!VK_CHECK(vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i])) ||
            !VK_CHECK(vkCreateSemaphore(device->vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i])) ||
            !VK_CHECK(vkCreateFence(device->vkDevice, &fenceInfo, nullptr, &inFlightFences[i]))) {

            LOG_ERROR("Failed to create synchronization objects for a frame!");
            return false;
        }
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying synchronization objects...");
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(device->vkDevice, imageAvailableSemaphores[i], nullptr);
                vkDestroySemaphore(device->vkDevice, renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(device->vkDevice, inFlightFences[i], nullptr);
            }
        }
    );

    return true;
}

VkCommandBuffer vax::vk::Engine::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->vkDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void vax::vk::Engine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queueManager->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queueManager->graphicsQueue);

    vkFreeCommandBuffers(device->vkDevice, commandPool, 1, &commandBuffer);
}

VkResult vax::vk::Engine::createAllocator() {
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = device->vkPhysicalDevice;
    allocatorInfo.device = device->vkDevice;
    allocatorInfo.instance = instance;
    allocatorInfo.vulkanApiVersion = vulkanApiVersion;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    return vmaCreateAllocator(&allocatorInfo, &allocator);
}