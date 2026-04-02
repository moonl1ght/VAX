#pragma once

#include "Renderer.hpp"
#include "vkEngine.h"
#include "luna.h"

class App final {
public:
    App() {};
    ~App() {};

    void run();

    vax::VkEngine* getEngine() const {
        return _engine;
    }

private:
    SDL_Window* _window = nullptr;
    vax::VkEngine* _engine = nullptr;
    Renderer* _renderer = nullptr;
    Scene* _scene = nullptr;

    bool setup();
    bool initWindow();
    void mainLoop();
    void cleanup();
    void loopUpdate();
};