#ifndef App_hpp
#define App_hpp

#include "Renderer.hpp"
#include "VKStack.hpp"
#include "DescriptorSetManager.hpp"
#include "PipelineManager.hpp"
#include "Primitives2D.hpp"
#include "luna.h"
#include "Buffer.hpp"
#include "Sampler.hpp"
#include "../shaders/VertexUniforms.h"
#include "TextureLoader.hpp"
#include "Logger.hpp"

class App final {
public:
    bool framebufferResized = false;

    App() { renderer = Renderer(); };
    ~App() {};

    void run();

protected:
    GLFWwindow* _window = nullptr;
    VKStack* _vkStack = nullptr;
    DescriptorSetManager* _descriptorSetManager = nullptr;
    PipelineManager* _pipelineManager = nullptr;

    Texture* texture = nullptr;

    Mesh* mesh;
    Renderer renderer;

    std::vector<Buffer*> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;

    // std::vector<VkDescriptorSet> descriptorSets;

    void setup();
    void initWindow();
    void mainLoop();
    void cleanup();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void updateUniformBuffer(uint32_t currentImage);
    // void createDescriptorSets();
};

#endif