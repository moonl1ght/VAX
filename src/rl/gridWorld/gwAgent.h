#pragma once

#include "drawableModel.h"
#include "luna.h"
#include "rlMathUtils.h"

namespace vax::rl::env::gw {
class Agent final {
    enum class Action : uint8_t {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
    };

  public:
    Agent();
    ~Agent();

    vax::objects::DrawableModel::LoadDescriptor getDrawableDescriptor() const;

  private:
    vax::rl::math::utils::Position2D _position;
};
} // namespace vax::rl::env::gw