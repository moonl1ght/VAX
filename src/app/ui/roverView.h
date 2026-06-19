#pragma once

#include "uiEngine.h"

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

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
};
} // namespace vax::ui