#pragma once

#include "uiEngine.h"
#include "gwTrainingManager.h"
#include "threadRunner.h"

namespace vax::ui {
class TrainingView final {
  public:
    TrainingView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~TrainingView() = default;

    TrainingView(const TrainingView& other) = delete;
    TrainingView& operator=(const TrainingView& other) = delete;
    TrainingView(TrainingView&& other) noexcept = delete;
    TrainingView& operator=(TrainingView&& other) noexcept = delete;

    void updateImGui();

    void startTraining();

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
    std::unique_ptr<vax::rl::GWTrainingManager> _trainingManager;
    vax::core::ThreadRunner _mainThreadRunner;
    std::string _trainingStatus;
};
} // namespace vax::ui