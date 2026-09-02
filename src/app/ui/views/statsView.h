#pragma once

#include "systemInfo.h"
#include "view.h"

namespace vax::ui {
class StatsView final : public View {
  public:
    StatsView(vax::engine::Renderer& renderer)
        : View(renderer) {
        _systemInfo = SystemInfo();
    };

    ~StatsView() = default;

    void update(const vax::engine::FrameTime& frameTime) override;

  private:
    SystemInfo _systemInfo;

    void showSystemInfo() const;
};
} // namespace vax::ui