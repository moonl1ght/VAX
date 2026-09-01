#pragma once

#include "drawableScene.h"
#include "gridWorld.h"
#include "gwTrainingManager.h"
#include "inputController.h"
#include "threadRunner.h"
#include "uiEngine.h"
#include "vkEngine.h"
#include "windowController.h"
#include "view.h"
#include "renderer.h"
#include "logger.h"

namespace vax::ui {
class RoverView final : public View {
  public:
    RoverView(UIEngine& uiEngine, vax::WindowController& windowController, vax::engine::Renderer& renderer)
        : _uiEngine(uiEngine)
        , _windowController(windowController)
        , _renderer(renderer) {}
    ~RoverView();

    RoverView(const RoverView& other) = delete;
    RoverView& operator=(const RoverView& other) = delete;
    RoverView(RoverView&& other) noexcept = delete;
    RoverView& operator=(RoverView&& other) noexcept = delete;

    void update(const vax::engine::FrameTime& frameTime) override;

    vax::AppMode getNextAppMode() override { return vax::AppMode::Demo; }

    void load(vax::vk::Engine& engine, vax::InputController& inputController);

  private:
    vax::Logger _logger = vax::Logger("RoverView");
    std::reference_wrapper<UIEngine> _uiEngine;
    std::reference_wrapper<vax::WindowController> _windowController;
    std::reference_wrapper<vax::engine::Renderer> _renderer;

    std::unique_ptr<vax::engine::DrawableScene> _drawableScene;
    std::unique_ptr<vax::rl::GridWorld> _gridWorld;
    std::unique_ptr<vax::rl::GWTrainingManager> _trainingManager;
    vax::core::ThreadRunner _mainThreadRunner;
    std::string _trainingStatus = "Training not started";
    bool _isTrainingRunning = false;
    bool _isDemoLoaded = false;
    bool _isDemoRunning = false;
    bool _isTrainingCompleted = false;
    bool _isRoverCameraShown = false;

    void _startTraining();

    void _toggleDemo();

    void _reinitGrid();

    void _startDemo();

    void _changeStartPosition();

    void _showRoverCamera();

    void _drawScene(const vax::engine::FrameTime& frameTime);
};
} // namespace vax::ui