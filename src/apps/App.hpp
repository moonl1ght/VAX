#ifndef App_hpp
#define App_hpp

#include "../renderer/Renderer.hpp"
#include "../vk/stack/VKStack.hpp"
#include "luna.h"

class App {
public:
    bool framebufferResized = false;

    App() { renderer = Renderer(); };
    virtual ~App() {};

    virtual void run();

protected:

    GLFWwindow *window = nullptr;
    VKStack *vkStack = nullptr;
    Renderer renderer;

    virtual void initWindow();
    virtual void mainLoop();
    virtual void cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
};

#endif