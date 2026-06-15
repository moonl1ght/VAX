#pragma once

#include "agent.h"
#include "loaderDescriptor.h"
#include "logger.h"
#include "luna.h"
#include "qlConfig.h"
#include "rlMath.h"
#include "tensor.h"
#include "gwenv.h"

namespace vax::rl::gw::env {
class GridWorld;
}

namespace vax::rl::gw {
class Agent final : public vax::rl::Agent<Agent, State, MoveAction> {
  public:
    Agent(vax::rl::ql::QLearningConfig qlConfig)
        : _qlConfig(qlConfig) {};
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

    void setStartPosition(int x, int y) {
        _startPosition = {x, y};
        _position = _startPosition;
    }

    vax::rl::math::Position2DInt getNewPosition(MoveAction action) const;

    MoveAction chooseActionImpl(const State& state);

    void updateImpl(const State& state, MoveAction action, double reward, const State& nextState, bool done);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("GWAgent");
    vax::rl::ql::QLearningConfig _qlConfig;
    vax::rl::math::Position2DInt _startPosition = {0, 0};
    vax::rl::math::Position2DInt _position = {0, 0};
    vax::rl::math::Position2DInt _oldPosition = {0, 0};
    vax::rl::gw::env::GridWorld* _gridWorld = nullptr;

    vax::math::Tensor _qTable;

    void _tryToMove(MoveAction action);
    bool _canTakeAction(MoveAction action) const;
};
} // namespace vax::rl::gw