#pragma once

#include "loaderDescriptor.h"
#include "luna.h"
#include "rlMath.h"
#include "logger.h"

namespace vax::rl::gw::env {
class GridWorld;
}

namespace vax::rl::gw {
class Agent final {
  public:
    enum class MoveAction : uint8_t {
        NORTH = 0,
        SOUTH = 1,
        EAST = 2,
        WEST = 3,
    };

    Agent() {};
    ~Agent() = default;

    Agent(const Agent& other) = delete;
    Agent& operator=(const Agent& other) = delete;
    Agent(Agent&& other) noexcept = delete;
    Agent& operator=(Agent&& other) noexcept = delete;

    vax::objects::LoaderDescriptor getDrawableDescriptor() const;

    void moveByOutsideAction(MoveAction action);

    void linkGridWorld(vax::rl::gw::env::GridWorld* gridWorld) { _gridWorld = gridWorld; }

    const vax::rl::math::Position2DInt& getPosition() const;
    const vax::rl::math::Position2DInt& getOldPosition() const;

    void setPosition(int x, int y) { _position = {x, y}; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("Agent");
    vax::rl::math::Position2DInt _position = {0, 0};
    vax::rl::math::Position2DInt _oldPosition = {0, 0};
    vax::rl::gw::env::GridWorld* _gridWorld = nullptr;

    void _tryToMove(MoveAction action);
    bool _canTakeAction(MoveAction action) const;
    vax::rl::math::Position2DInt _getNewPosition(MoveAction action) const;
};
} // namespace vax::rl::gw