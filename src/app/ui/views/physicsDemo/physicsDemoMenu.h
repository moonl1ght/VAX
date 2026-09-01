#pragma once

#include "uiEngine.h"

namespace vax::ui {
class PhysicsDemoMenuView final {
  public:
    enum class Action {
      GO_TO_MAIN_MENU = 0,
      SHOW_SIMPLE_DEMO = 1,
    };

    PhysicsDemoMenuView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~PhysicsDemoMenuView() = default;

    PhysicsDemoMenuView(const PhysicsDemoMenuView& other) = delete;
    PhysicsDemoMenuView& operator=(const PhysicsDemoMenuView& other) = delete;
    PhysicsDemoMenuView(PhysicsDemoMenuView&& other) noexcept = delete;
    PhysicsDemoMenuView& operator=(PhysicsDemoMenuView&& other) noexcept = delete;

    void updateImGui();
    std::optional<Action> popPendingAction() { return std::exchange(_pendingAction, std::nullopt); }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
    std::optional<Action> _pendingAction;
};
} // namespace vax::ui