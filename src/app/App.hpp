#ifndef App_hpp
#define App_hpp

#include "Renderer.hpp"
#include "VKEngine.hpp"
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
    VKEngine* getVKEngine() const {
        return _vkEngine;
    }

private:
    GLFWwindow* _window = nullptr;
    VKEngine* _vkEngine = nullptr;
    DescriptorSetManager* _descriptorSetManager = nullptr;
    PipelineManager* _pipelineManager = nullptr;
    Renderer* _renderer = nullptr;
    Scene* _scene = nullptr;

    void setup();
    void initWindow();
    void mainLoop();
    void cleanup();
    void loopUpdate();
};

#endif