#ifndef App_hpp
#define App_hpp

#include "luna.h"
#include "../vk/stack/VKStack.hpp"

class App {
    public:

    virtual void run();

    protected:

    const bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    GLFWwindow* window = nullptr;
    VKStack* vkStack = nullptr;

    virtual void initWindow();
    virtual void mainLoop();
    virtual void cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
};

#endif