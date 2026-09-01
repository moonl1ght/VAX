#pragma once

#include "trainingView.h"
#include "view.h"

namespace vax::ui {
class MenuView final : public View {
  public:
    enum class Action {
        SHOW_ROVER_DEMO,
        TRAIN_Q_LEARNING,
        SHOW_PHYSICS_ENGINE_DEMO,
    };

    MenuView() {};

    ~MenuView() override = default;

    MenuView(const MenuView& other) = delete;
    MenuView& operator=(const MenuView& other) = delete;
    MenuView(MenuView&& other) noexcept = delete;
    MenuView& operator=(MenuView&& other) noexcept = delete;

    void update(const vax::engine::FrameTime& frameTime) override;

    vax::AppMode getNextAppMode() override;

  private:
    std::unique_ptr<TrainingView> _trainingView = nullptr;
    std::optional<Action> _pendingAction;
    bool _showTrainingStatus = false;

    std::optional<Action> _popPendingAction() { return std::exchange(_pendingAction, std::nullopt); }
};
} // namespace vax::ui