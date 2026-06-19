#include "gwAgent.h"
#include "gridWorld.h"
#include "gwenv.h"
#include "randomGenerator.h"
#include "tensorOp.h"

using namespace vax::rl::gw;
using namespace vax;
using namespace vax::rl::gw::env;
using namespace vax::rl::math;
using namespace vax::math;
using namespace vax::utils;

vax::objects::LoaderDescriptor Agent::getDrawableDescriptor() const {
    // TODO: check if initial transform affects the model
    return {
        RES_PATH("assets/models/rover/rover.urdf"),
        {},
    };
}

void Agent::allowAction(MoveAction action) {
    _oldPosition = _position;
    _position = getNewPosition(action);
    if (_evalMode == vax::rl::EvalMode::EVALUATION) {
        _gridWorld->agentMoved();
    }
}

void Agent::linkGridWorld(vax::rl::gw::env::GridWorld* gridWorld) {
    _gridWorld = gridWorld;
    _qTable = Tensor::createZeros({_gridWorld->getGrid().totalSize(), numMoveActions});
}

void Agent::moveByOutsideAction(MoveAction action) { _tryToMove(action); }

void Agent::_tryToMove(MoveAction action) {
    if (_canTakeAction(action)) {
        _oldPosition = _position;
        _position = getNewPosition(action);
        if (_evalMode == vax::rl::EvalMode::EVALUATION) {
            _gridWorld->agentMoved();
        }
    }
}

bool Agent::_canTakeAction(MoveAction action) const {
    Position2DInt newPosition = getNewPosition(action);
    if (_gridWorld) {
        return _gridWorld->canMoveAgent(newPosition);
    }
    _logger.warning("Grid world not linked!");
    return false;
}

Position2DInt Agent::getNewPosition(MoveAction action) const {
    Position2DInt newPosition = _position;
    switch (action) {
    case MoveAction::NORTH:
        newPosition.y += 1;
        break;
    case MoveAction::SOUTH:
        newPosition.y -= 1;
        break;
    case MoveAction::EAST:
        newPosition.x -= 1;
        break;
    case MoveAction::WEST:
        newPosition.x += 1;
        break;
    }
    return newPosition;
}

const Position2DInt& Agent::getPosition() const { return _position; }

const Position2DInt& Agent::getOldPosition() const { return _oldPosition; }

MoveAction Agent::chooseActionImpl(const State& state) {
    core::RandomGenerator& generator = core::RandomGenerator::getInstance();
    if (generator.uniformFloat() < _qlConfig.epsilon) {
        auto action = static_cast<MoveAction>(generator.uniformInt(0, numMoveActions - 1));
        return action;
    }
    auto flatIndex = _gridWorld->getGrid().flatIndex({state.x, state.y});
    auto indices = TensorOp::maxOverLastDim(_qTable, {flatIndex});
    if (indices.size() != 2) {
        _logger.error("No action found for state: ", state.x, ", ", state.y);
        return MoveAction::NORTH;
    }
    auto action = static_cast<MoveAction>(indices[1]);
    return action;
}

void Agent::updateImpl(const State& state, MoveAction action, double reward, const State& nextState, bool done) {
    float maxFuture = 0.0f;
    auto flatIndexNextState = _gridWorld->getGrid().flatIndex({nextState.x, nextState.y});
    auto flatIndexState = _gridWorld->getGrid().flatIndex({state.x, state.y});
    if (!done) {
        auto indices = TensorOp::maxOverLastDim(_qTable, {flatIndexNextState});
        if (indices.empty()) {
            _logger.error("No action found for next state: ", nextState.x, ", ", nextState.y);
            return;
        }
        auto maxFutureValue = _qTable.get(indices);
        if (!maxFutureValue.has_value()) {
            _logger.error("No max future value found for next state: ", nextState.x, ", ", nextState.y);
            return;
        }
        maxFuture = maxFutureValue.value();
    }

    float target = reward + _qlConfig.gamma * maxFuture;

    auto prevValue = _qTable.get({flatIndexState, static_cast<int>(action)});
    if (!prevValue.has_value()) {
        _logger.error(
            "No previous value found for state: ", state.x, ", ", state.y, " and action: ", static_cast<int>(action)
        );
        return;
    }
    auto updatedValue = _qlConfig.learningRate * (target - prevValue.value());
    _qTable.set({flatIndexState, static_cast<int>(action)}, updatedValue);
}

void Agent::reset() {
    _position = _startPosition;
    _oldPosition = _startPosition;
}

void Agent::setEvalModeImpl(vax::rl::EvalMode evalMode) { _evalMode = evalMode; }