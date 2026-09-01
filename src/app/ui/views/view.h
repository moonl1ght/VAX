#pragma once

#pragma once

#include "appMode.h"
#include "frameTime.h"

namespace vax::ui {
class View {
  public:
    View() {};

    virtual ~View() = default;

    virtual void update(const vax::engine::FrameTime& frameTime) = 0;

    virtual vax::AppMode getNextAppMode() = 0;
};
} // namespace vax::ui