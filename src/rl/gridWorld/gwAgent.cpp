#include "gwAgent.h"
#include "gridWorld.h"
#include "gwenv.h"
#include "randomGenerator.h"
#include "transform.h"

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
    // core::RandomGenerator& generator = core::RandomGenerator::getInstance();
    // if (generator.uniformFloat() < _qlConfig.epsilon) {
    //     return generator.uniformInt(0, numMoveActions - 1);
    // }
    // return std::distance(qTable[state].begin(), std::max_element(qTable[state].begin(), qTable[state].end()));
    return MoveAction::NORTH;
}

void Agent::updateImpl(const State& state, MoveAction action, double reward, const State& nextState, bool done) {
    // TODO: implement
}