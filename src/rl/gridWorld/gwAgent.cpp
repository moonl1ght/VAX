#include "gwAgent.h"
#include "gridWorld.h"
#include "gwenv.h"
#include "randomGenerator.h"
#include "transform.h"
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
    _gridWorld->agentMoved();
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
        _gridWorld->agentMoved();
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
        return static_cast<MoveAction>(generator.uniformInt(0, numMoveActions - 1));
    }
    // return std::distance(qTable[state].begin(), std::max_element(qTable[state].begin(), qTable[state].end()));
    return MoveAction::NORTH;
}

void Agent::updateImpl(const State& state, MoveAction action, double reward, const State& nextState, bool done) {
    double maxFuture = 0.0;
    if (!done) {
        // int maxIndex = TensorOp::argmax(_qTable);
        // if (maxIndex != -1) {
        //     maxFuture = _qTable.data()[maxIndex];
        // }
    }

    double target = reward + _qlConfig.gamma * maxFuture;

    // q_table[state][action] += config.alpha * (target - q_table[state][action]);
}

void Agent::reset() {
    _position = _startPosition;
    _oldPosition = _startPosition;
}