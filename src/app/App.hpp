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
#include "../shaders/ShaderUniforms.h"
#include "TextureLoader.hpp"
#include "Logger.hpp"
#include "DrawableModel.hpp"

class App final {
public:
    App() { };
    ~App() { };

    void run();
    VKStack* getVKStack() const {
        return _vkStack;
    }

private:
    GLFWwindow* _window = nullptr;
    VKStack* _vkStack = nullptr;
    DescriptorSetManager* _descriptorSetManager = nullptr;
    PipelineManager* _pipelineManager = nullptr;
    Renderer* _renderer = nullptr;
    Scene* _scene = nullptr;

    DrawableModel* model = nullptr;
    // Buffer* buffer = nullptr;

    // std::vector<Buffer*> uniformBuffers;
    // std::vector<Buffer*> objectUniformBuffers;
    // std::vector<void*> uniformBuffersMapped;
    // std::vector<void*> objectUniformBuffersMapped;

    void setup();
    void initWindow();
    void mainLoop();
    void cleanup();
    void loopUpdate();

    void updateUniformBuffer(VkCommandBuffer commandBuffer, uint32_t currentImage);
};

#endif