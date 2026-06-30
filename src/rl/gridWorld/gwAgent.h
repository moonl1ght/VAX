#pragma once

#include "agent.h"
#include "gwenv.h"
#include "modelDescriptor.h"
#include "logger.h"
#include "luna.h"
#include "qlConfig.h"
#include "vaxMath.h"
#include "tensor.h"

namespace vax::rl::gw::env {
class GridWorld;
}

namespace vax::rl::gw {
class Agent final : public vax::rl::Agent<Agent, State, MoveAction> {
  public:
    explicit Agent(vax::rl::ql::QLearningConfig qlConfig)
        : _qlConfig(qlConfig) {};

    Agent(
        vax::rl::ql::QLearningConfig qlConfig,
        const vax::math::Position2DInt& startPosition,
        vax::math::Tensor&& qTable
    )
        : _qlConfig(qlConfig)
        , _startPosition(startPosition)
        , _qTable(std::move(qTable)) {};

    ~Agent() = default;

    Agent(const Agent& other) = delete;
    Agent& operator=(const Agent& other) = delete;
    Agent(Agent&& other) noexcept = delete;
    Agent& operator=(Agent&& other) noexcept = delete;

    vax::objects::ModelDescriptor getDrawableDescriptor() const;

    void moveByOutsideAction(MoveAction action);

    void allowAction(MoveAction action);

    void linkGridWorld(vax::rl::gw::env::GridWorld* gridWorld);

    const vax::math::Position2DInt& getPosition() const;
    const vax::math::Position2DInt& getOldPosition() const;

    void setStartPosition(int x, int y) {
        _startPosition = {x, y};
        _position = _startPosition;
    }

    vax::math::Position2DInt getNewPosition(MoveAction action) const;

    MoveAction chooseActionImpl(const State& state);

    void updateImpl(const State& state, MoveAction action, double reward, const State& nextState, bool done);

    void reset();

    void setEvalModeImpl(vax::rl::EvalMode evalMode);

    const vax::math::Tensor& getQTable() const { return _qTable; }

    const vax::math::Position2DInt& getStartPosition() const { return _startPosition; }

    void setQTable(vax::math::Tensor&& qTable);

    void setQLearningConfig(const vax::rl::ql::QLearningConfig& qlConfig);

    void setFsLogger(std::shared_ptr<vax::FsLogger> fsLogger);

    AgentOrientation getOrientation() const { return _orientation; }

  private:
    vax::Logger _logger = vax::Logger("GWAgent");
    vax::rl::ql::QLearningConfig _qlConfig;
    vax::math::Position2DInt _startPosition = {0, 0};
    vax::math::Position2DInt _position = {0, 0};
    vax::math::Position2DInt _oldPosition = {0, 0};
    vax::rl::gw::env::GridWorld* _gridWorld = nullptr;
    vax::rl::EvalMode _evalMode = vax::rl::EvalMode::EVALUATION;
    AgentOrientation _orientation = AgentOrientation::NORTH;

    vax::math::Tensor _qTable;

    void _tryToMove(MoveAction action);
    bool _canTakeAction(MoveAction action) const;
    void _updateOrientation(MoveAction action);
};
} // namespace vax::rl::gw