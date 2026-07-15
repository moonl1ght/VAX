#pragma once

#include "uiEngine.h"
#include "drawableScene.h"
#include "gridWorld.h"
#include "inputController.h"
#include "vkEngine.h"
#include "threadRunner.h"
#include "gwTrainingManager.h"

namespace vax::ui {
class RoverView final {
  public:
    RoverView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~RoverView() = default;

    RoverView(const RoverView& other) = delete;
    RoverView& operator=(const RoverView& other) = delete;
    RoverView(RoverView&& other) noexcept = delete;
    RoverView& operator=(RoverView&& other) noexcept = delete;

    void updateImGui();
    void load(vax::vk::Engine& engine, vax::InputController& inputController);

    vax::engine::DrawableScene* drawableScene() const { return _drawableScene.get(); }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;

    std::unique_ptr<vax::engine::DrawableScene> _drawableScene;
    std::unique_ptr<vax::rl::GridWorld> _gridWorld;
    std::unique_ptr<vax::rl::GWTrainingManager> _trainingManager;
    vax::core::ThreadRunner _mainThreadRunner;
    std::string _trainingStatus = "Training not started";
    bool _isTrainingRunning = false;
    bool _isDemoLoaded = false;
    bool _isDemoRunning = false;
    bool _isTrainingCompleted = false;

    void _startTraining();

    void _toggleDemo();

    void _reinitGrid();

    void _startDemo();

    void _changeStartPosition();
};
} // namespace vax::ui