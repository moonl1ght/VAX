#include "vkEngine.h"
#include "textureLoader.h"
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

    deletionQueue.push_function(
        [&]() {
            queueManager = nullptr;
        }
    );

    _logger.info("Creating allocator...");
    if (!VK_CHECK(createAllocator())) {
        _logger.error("Failed to create allocator!");
        return false;
    }

    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying allocator...");
            vmaDestroyAllocator(allocator);
        }
    );

    commandManager = std::make_unique<CommandManager>(*device);
    if (!commandManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            commandManager->cleanup();
        }
    );

    syncObjectsManager = std::make_unique<SyncObjectsManager>(*device);
    if (!syncObjectsManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            syncObjectsManager->cleanup();
        }
    );

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
    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying render pass...");
            renderPass = nullptr; // TODO: remove from destructor
        }
    );

    auto renderDestinationOptional = RenderDestinationBuilder(
        *device,
        allocator,
        *swapchain,
        *renderPass
    ).build(this);
    if (!renderDestinationOptional.has_value()) return false;
    renderDestination = std::move(*renderDestinationOptional);
    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying render destination...");
            renderDestination = nullptr; // TODO: remove from destructor
        }
    );

    descriptorSetManager = std::make_unique<DescriptorSetManager>(this);
    if (!descriptorSetManager->setup()) return false;
    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying descriptor set manager...");
            descriptorSetManager = nullptr; // TODO: remove from destructor
        }
    );

    pipelineManager = std::make_unique<PipelineManager>(*device, *descriptorSetManager);
    pipelineManager->setup(*renderPass);
    deletionQueue.push_function(
        [&]() {
            _logger.info("Destroying pipeline manager...");
            pipelineManager = nullptr; // TODO: remove from destructor
        }
    );

    _logger.info("Engine setup complete!");
    return true;
}

void vax::vk::Engine::cleanup() {
    deletionQueue.flush();
    _logger.info("Engine cleanup complete!");
}

void vax::vk::Engine::resize() {
    _logger.info("Resizing engine...");
    // int width = 0, height = 0;
    // SDL_GetWindowSizeInPixels(_window.get().window, &width, &height);
    // while (width == 0 || height == 0) {
    //     SDL_GetWindowSizeInPixels(_window.get().window, &width, &height);
    //     SDL_Delay(100);
    // }
    // vkDeviceWaitIdle(device->vkDevice);

    // swapchain->recreate();
    // auto renderDestinationOptional = RenderDestinationBuilder(
    //     *device,
    //     allocator,
    //     *swapchain,
    //     *renderPass
    // ).build(this);
    // if (!renderDestinationOptional.has_value()) return;
    // renderDestination = std::move(*renderDestinationOptional);
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

VkResult vax::vk::Engine::createAllocator() {
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = device->vkPhysicalDevice;
    allocatorInfo.device = device->vkDevice;
    allocatorInfo.instance = instance;
    allocatorInfo.vulkanApiVersion = vulkanApiVersion;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    return vmaCreateAllocator(&allocatorInfo, &allocator);
}