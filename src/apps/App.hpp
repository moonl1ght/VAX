#ifndef App_hpp
#define App_hpp

#include "Renderer.hpp"
#include "VKStack.hpp"
#include "Primitives2D.hpp"
#include "luna.h"

class App {
public:
    bool framebufferResized = false;

    App() { renderer = Renderer(); };
    virtual ~App() {};

    virtual void run();

protected:

    Mesh* mesh;
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