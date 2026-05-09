#pragma once

#include "renderer.h"
#include "vkEngine.h"
#include "luna.h"
#include "window.h"
#include "logger.h"
#include "uiLayer.h"
#include "drawableScene.h"
#include "inputController.h"

namespace vax {
    class App final {
    public:
        App() {
            _inputController = vax::input::InputController();
        };
        ~App() {};

        bool run();

        vax::vk::Engine* getEngine() const {
            return _engine.get();
        }

    private:
        utils::Logger _logger = utils::Logger("App");

        vax::input::InputController _inputController;

        std::unique_ptr<vax::vk::Window> _window;
        std::unique_ptr<vax::vk::Engine> _engine;
        std::unique_ptr<vax::renderer::Renderer> _renderer;
        std::unique_ptr<DrawableScene> _drawableScene;
        std::unique_ptr<vax::ui::UILayer> _uiLayer;

        bool setup();
        void mainLoop();
        void cleanup();
        void loopUpdate();
    };
}