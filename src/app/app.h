#pragma once

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
#include "frameTime.h"
#include "windowController.h"

namespace vax {
enum class AppMode { Menu, Demo, Training };

class App final {
  public:
    App() { _inputController = vax::InputController(); };
    ~App() {};

    bool run();

    vax::vk::Engine* getEngine() const { return _engine.get(); }

  private:
    vax::Logger _logger = vax::Logger("App");

    vax::InputController _inputController;
    vax::AppMode _appMode = vax::AppMode::Menu;
    vax::engine::FrameTime _frameTime;

    std::unique_ptr<vax::WindowController> _windowController;
    std::unique_ptr<vax::vk::Engine> _engine;
    std::unique_ptr<vax::engine::Renderer> _renderer;
    std::unique_ptr<vax::ui::UIEngine> _uiEngine;
    std::unique_ptr<vax::ui::MenuView> _menuView;
    std::unique_ptr<vax::ui::RoverView> _roverView;
    std::unique_ptr<vax::ui::TrainingView> _trainingView;

    bool _setup();
    void _mainLoop();
    void _cleanup();
    void _loopByEventUpdate();
    void _loopContinuousUpdate();
    void _updateTimestamp();
    void _checkActions();
    void _printGpuInfo() const;
};
} // namespace vax