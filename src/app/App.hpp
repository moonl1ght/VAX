#pragma once

#include "Renderer.hpp"
#include "vkEngine.h"
#include "luna.h"
#include "window.h"

class App final {
public:
    App() {};
    ~App() {};

    void run();

    vax::vk::Engine* getEngine() const {
        return _engine.get();
    }

private:
    std::unique_ptr<vax::vk::Window> _window;
    std::unique_ptr<vax::vk::Engine> _engine;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<Scene> _scene;

    bool setup();
    void mainLoop();
    void cleanup();
    void loopUpdate();
};