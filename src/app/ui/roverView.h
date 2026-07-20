#pragma once

#include "drawableScene.h"
#include "gridWorld.h"
#include "gwTrainingManager.h"
#include "inputController.h"
#include "threadRunner.h"
#include "uiEngine.h"
#include "vkEngine.h"
#include "windowController.h"

namespace vax::ui {
class RoverView final {
  public:
    RoverView(UIEngine& uiEngine, vax::WindowController& windowController)
        : _uiEngine(uiEngine)
        , _windowController(windowController) {}
    ~RoverView();

    RoverView(const RoverView& other) = delete;
    RoverView& operator=(const RoverView& other) = delete;
    RoverView(RoverView&& other) noexcept = delete;
    RoverView& operator=(RoverView&& other) noexcept = delete;

    void updateImGui();
    void load(vax::vk::Engine& engine, vax::InputController& inputController);

    vax::engine::DrawableScene* drawableScene() const { return _drawableScene.get(); }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
    std::reference_wrapper<vax::WindowController> _windowController;

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
};
} // namespace vax::ui