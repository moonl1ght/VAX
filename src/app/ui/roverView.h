#pragma once

namespace vax::ui {
class RoverView final {
  public:
    RoverView() = default;
    ~RoverView() = default;

    RoverView(const RoverView& other) = delete;
    RoverView& operator=(const RoverView& other) = delete;
    RoverView(RoverView&& other) noexcept = delete;
    RoverView& operator=(RoverView&& other) noexcept = delete;
};
} // namespace vax::ui