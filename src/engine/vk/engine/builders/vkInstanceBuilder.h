#pragma once

#include "luna.h"
#include "deletionQueue.h"

namespace vax {
    class VkInstanceBuilder {
    public:
        VkInstanceBuilder(
            DeletionQueue& deletionQueue,
            const bool enableValidationLayers,
            const std::vector<const char*>& validationLayers,
            const uint32_t vulkanApiVersion
        ) :
            deletionQueue(deletionQueue),
            enableValidationLayers(enableValidationLayers),
            validationLayers(validationLayers),
            vulkanApiVersion(vulkanApiVersion) {
            logger = Logger("VkInstanceBuilder");
        };

        std::optional<VkInstance> build() {
            logger.info("Creating instance...");
            if (enableValidationLayers && !checkValidationLayerSupport()) {
                logger.error("Validation layers requested, but not available!");
                return std::nullopt;
            }
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "Luna";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "Luna Engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = vulkanApiVersion;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

            auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                vax::populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
            }
            else {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            VkInstance instance = VK_NULL_HANDLE;

            if (!VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance))) {
                logger.error("Failed to create instance!");
                return std::nullopt;
            }

            return std::optional<VkInstance>(instance);
        }

    private:
        Logger logger;
        DeletionQueue& deletionQueue;
        const bool enableValidationLayers;
        const std::vector<const char*>& validationLayers;
        const uint32_t vulkanApiVersion;

        std::vector<const char*> getRequiredExtensions() const {
            unsigned int sdl_extensions_count = 0;
            SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
            const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);

            std::vector<const char*> extensions(sdl_extensions, sdl_extensions + sdl_extensions_count);

            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
            // extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

            return extensions;
        }

        bool checkValidationLayerSupport() const {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }

            return true;
        }
    };
}