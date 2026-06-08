#pragma once

#include "loaderDescriptor.h"
#include "luna.h"
#include "rlMathUtils.h"

namespace vax::rl::env::gw {
class Agent final {
    enum class Action : uint8_t {
        NORTH = 0,
        SOUTH = 1,
        EAST = 2,
        WEST = 3,
    };

  public:
    Agent();
    ~Agent();

    vax::objects::LoaderDescriptor getDrawableDescriptor() const;

  private:
    vax::rl::math::utils::Position2D _position;
};
} // namespace vax::rl::env::gw