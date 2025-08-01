#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Renderer.hpp"
#include "VKEngine.hpp"
#include "luna.h"
#include "Logger.hpp"

class App final {
public:
    App() {};
    ~App() {};

    void run();

    VKEngine* getEngine() const {
        return _engine;
    }

private:
    SDL_Window* _window = nullptr;
    VKEngine* _engine = nullptr;
    Renderer* _renderer = nullptr;
    Scene* _scene = nullptr;

    bool setup();
    bool initWindow();
    void mainLoop();
    void cleanup();
    void loopUpdate();
};