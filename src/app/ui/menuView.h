#pragma once

#include "uiEngine.h"

namespace vax::ui {
class MenuView final {
  public:
    enum class Action {
        SHOW_ROVER_DEMO,
        TRAIN_Q_LEARNING,
    };

    MenuView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~MenuView() = default;

    MenuView(const MenuView& other) = delete;
    MenuView& operator=(const MenuView& other) = delete;
    MenuView(MenuView&& other) noexcept = delete;
    MenuView& operator=(MenuView&& other) noexcept = delete;

    void updateImGui();
    std::optional<Action> popPendingAction() { return std::exchange(_pendingAction, std::nullopt); }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
    std::optional<Action> _pendingAction;
};
} // namespace vax::ui