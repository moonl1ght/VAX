#include "vkEngine.h"
#include "TextureLoader.hpp"
#include "Texture.hpp"
#include "SwapchainManager.hpp"
#include "RenderingDestination.hpp"
#include "DescriptorSetManager.hpp"
#include "PipelineManager.hpp"
#include "vk_debug.h"
#include "vkInstanceBuilder.h"

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void validationLayersDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

bool vax::VkEngine::setup() {
    std::optional<VkInstance> instanceOptional = vax::VkInstanceBuilder(
        deletionQueue,
        enableValidationLayers,
        validationLayers,
        vulkanApiVersion
    ).build();
    if (instanceOptional.has_value()) {
        instance = *instanceOptional;
    } else {
        return false;
    }

    if (!setupDebugMessenger()) return false;

    printf("SDL version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    LOG_INFO("Creating surface...");
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        std::cout << "Failed to create surface! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying surface...");
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
    );

    LOG_INFO("Creating device...");
    device = new vax::Device();
    if (!device->load(instance, surface, enableValidationLayers)) {
        LOG_ERROR("Failed to create device!");
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying device...");
            delete device;
            device = nullptr;
        }
    );

    LOG_INFO("Getting device queues...");
    VKUtils::QueueFamilyIndices indices = device->getQueueFamilyIndices();
    vkGetDeviceQueue(device->vkDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device->vkDevice, indices.presentFamily.value(), 0, &presentQueue);

    LOG_INFO("Creating allocator...");
    if (!VK_CHECK(createAllocator())) {
        LOG_ERROR("Failed to create allocator!");
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            LOG_INFO("Destroying allocator...");
            vmaDestroyAllocator(allocator);
        }
    );

    if (!createCommandPool()) return false;

    if (!createCommandBuffer()) return false;

    if (!createSyncObjects()) return false;

    swapchainManager = new SwapchainManager(window, surface, device);
    if (!swapchainManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            swapchainManager->cleanup();
            delete swapchainManager;
            swapchainManager = nullptr;
        }
    );

    renderPassManager = new RenderPassManager(swapchainManager, device);
    if (!renderPassManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            renderPassManager->cleanup();
            delete renderPassManager;
            renderPassManager = nullptr;
        }
    );

    renderingDestination = new RenderingDestination(this, swapchainManager, renderPassManager);
    renderingDestination->setup();
    deletionQueue.push_function(
        [&]() {
            renderingDestination->cleanup();
            delete renderingDestination;
            renderingDestination = nullptr;
        }
    );

    descriptorSetManager = new DescriptorSetManager(this);
    if (!descriptorSetManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            delete descriptorSetManager;
            descriptorSetManager = nullptr;
        }
    );

    pipelineManager = new PipelineManager(this, descriptorSetManager);
    pipelineManager->setup();
    deletionQueue.push_function(
        [&]() {
            delete pipelineManager;
            pipelineManager = nullptr;
        }
    );

    LOG_INFO("Engine setup complete!");
    return true;
}

void vax::VkEngine::cleanup() {
    deletionQueue.flush();
    LOG_INFO("Engine cleanup complete!");
}

bool vax::VkEngine::setupDebugMessenger() {
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

bool vax::VkEngine::createCommandPool() {
    LOG_INFO("Creating command pool...");
    VKUtils::QueueFamilyIndices queueFamilyIndices = device->getQueueFamilyIndices();

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

bool vax::VkEngine::createCommandBuffer() {
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

bool vax::VkEngine::createSyncObjects() {
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

VkCommandBuffer vax::VkEngine::beginSingleTimeCommands() {
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

void vax::VkEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device->vkDevice, commandPool, 1, &commandBuffer);
}

VkResult vax::VkEngine::createAllocator() {
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = device->vkPhysicalDevice;
    allocatorInfo.device = device->vkDevice;
    allocatorInfo.instance = instance;
    allocatorInfo.vulkanApiVersion = vulkanApiVersion;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    return vmaCreateAllocator(&allocatorInfo, &allocator);
}