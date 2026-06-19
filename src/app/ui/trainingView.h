#pragma once

#include "uiEngine.h"

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
};
} // namespace vax::ui