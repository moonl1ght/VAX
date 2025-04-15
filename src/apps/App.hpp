#ifndef App_hpp
#define App_hpp

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>

class App {
    public:

    virtual void run();

    protected:

    const bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    GLFWwindow* window = nullptr;
    VkInstance instance;
    // VkDebugUtilsMessengerEXT debugMessenger;
    // VkSurfaceKHR surface;

    // VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    // VkDevice device;

    // VkQueue graphicsQueue;
    // VkQueue presentQueue;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
        void* pUserData
    ) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    virtual void initWindow();
    virtual void initVulkan();
    virtual void mainLoop();
    virtual void cleanup();
    virtual void createInstance();
    virtual bool checkValidationLayerSupport();
    virtual std::vector<const char*> getRequiredExtensions();
    virtual void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

#endif