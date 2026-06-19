#pragma once

#include "drawableScene.h"
#include "gridWorld.h"
#include "inputController.h"
#include "logger.h"
#include "luna.h"
#include "menuView.h"
#include "renderer.h"
#include "roverView.h"
#include "trainingView.h"
#include "uiEngine.h"
#include "vkEngine.h"
#include "window.h"

namespace vax {
enum class AppMode { Menu, RoverDemo, Training };

class App final {
  public:
    App() { _inputController = vax::input::InputController(); };
    ~App() {};

    bool run();

    vax::vk::Engine* getEngine() const { return _engine.get(); }

  private:
    utils::Logger _logger = utils::Logger("App");

    vax::input::InputController _inputController;
    vax::AppMode _appMode = vax::AppMode::Menu;
    float _timestamp = 0.0f;

    std::unique_ptr<vax::vk::Window> _window;
    std::unique_ptr<vax::vk::Engine> _engine;
    std::unique_ptr<vax::renderer::Renderer> _renderer;
    std::unique_ptr<DrawableScene> _drawableScene;
    std::unique_ptr<vax::ui::UIEngine> _uiEngine;
    std::unique_ptr<vax::rl::gw::env::GridWorld> _gridWorld;
    std::unique_ptr<vax::ui::MenuView> _menuView;
    std::unique_ptr<vax::ui::RoverView> _roverView;
    std::unique_ptr<vax::ui::TrainingView> _trainingView;

    bool _setup();
    void _mainLoop();
    void _cleanup();
    void _loopByEventUpdate();
    void _loopContinuousUpdate();
    void _updateTimestamp();
    void _updateAppMode();
};
} // namespace vax