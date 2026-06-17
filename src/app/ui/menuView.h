#pragma once

#include "uiEngine.h"

namespace vax::ui {
class MenuView final {
  public:
    MenuView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~MenuView() = default;

    MenuView(const MenuView& other) = delete;
    MenuView& operator=(const MenuView& other) = delete;
    MenuView(MenuView&& other) noexcept = delete;
    MenuView& operator=(MenuView&& other) noexcept = delete;

    void updateImGui();

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
};
} // namespace vax::ui