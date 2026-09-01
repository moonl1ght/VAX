#pragma once

#include "gwTrainingManager.h"
#include "threadRunner.h"
#include "view.h"

namespace vax::ui {
class TrainingView final : public View {
  public:
    TrainingView() {};

    ~TrainingView() = default;

    TrainingView(const TrainingView& other) = delete;
    TrainingView& operator=(const TrainingView& other) = delete;
    TrainingView(TrainingView&& other) noexcept = delete;
    TrainingView& operator=(TrainingView&& other) noexcept = delete;

    void update(const vax::engine::FrameTime& frameTime) override;

    vax::AppMode getNextAppMode() override { return vax::AppMode::Training; }

    void startTraining();

  private:
    std::unique_ptr<vax::rl::GWTrainingManager> _trainingManager;
    vax::core::ThreadRunner _mainThreadRunner;
    std::string _trainingStatus;
};
} // namespace vax::ui