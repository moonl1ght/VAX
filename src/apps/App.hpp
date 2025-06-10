#ifndef App_hpp
#define App_hpp

#include "Renderer.hpp"
#include "VKStack.hpp"
#include "Primitives2D.hpp"
#include "luna.h"
#include "Buffer.hpp"
#include "Sampler.hpp"
#include "../shaders/VertexUniforms.h"
#include "TextureLoader.hpp"

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

    std::vector<Buffer*> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;

    std::vector<VkDescriptorSet> descriptorSets;

    virtual void initWindow();
    virtual void mainLoop();
    virtual void cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void updateUniformBuffer(uint32_t currentImage);
    void createDescriptorSets();
};

#endif