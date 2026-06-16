#include "gridWorld.h"
#include "inputController.h"
#include "randomGenerator.h"
#include "rlMath.h"
#include "tensorOp.h"
#include "transform.h"
#include "trainingEngine.h"

using namespace vax::rl::gw::env;
using namespace vax::rl::gw;
using namespace vax;
using namespace vax::math;
using namespace vax::rl::math;
using namespace vax::rl;

void GridWorld::load() {
    _grid = Tensor::createZeros({6, 6});
    _sceneGraphPositions.reserve(_grid.totalSize());
    bool agentWasPlaced = false;
    for (int i = 0; i < _grid.totalSize(); ++i) {
        std::vector<int> indices = _grid.indices(i);
        auto padding = 1.0f;
        auto offset = 5.0f / 2.0f - 0.5f;
        Position2DFloat position = {indices[0] * padding - offset * padding, indices[1] * padding - offset * padding};
        _sceneGraphPositions.push_back(position);
        // TODO: rework this
        if (isBorderIndex(indices, _grid.shape())) {
            _grid.set(indices, static_cast<float>(BlockType::WALL));
        } else if (!agentWasPlaced && core::RandomGenerator::getInstance().uniformBool()) {
            agentWasPlaced = true;
            _agent.setStartPosition(indices[0], indices[1]);
        }
    }
    _agent.linkGridWorld(this);

    vax::rl::training::TrainingEngine trainingEngine;
    trainingEngine.train<GridWorld, Agent, State, MoveAction>(*this, _agent, 100);
}

void GridWorld::linkSceneGraph(GwSceneGraph* sceneGraph) {
    _sceneGraph = sceneGraph;
    auto position = _agent.getPosition();
    auto flatIndex = _grid.flatIndex({position.x, position.y});
    auto sceneGraphPosition = _sceneGraphPositions[_grid.flatIndex({position.x, position.y})];
    _sceneGraph->moveAgent(sceneGraphPosition);
}

GridWorldDrawableDescriptor GridWorld::getDrawableDescriptor() const {
    GridWorldDrawableDescriptor descriptor;
    descriptor.drawableDescriptors.reserve(_grid.totalSize());
    int flatIndex = 0;
    for (const auto& block : _grid) {
        BlockType blockType = static_cast<BlockType>(block);
        Transform transform = Transform();
        transform.position = {_sceneGraphPositions[flatIndex].x, 0.0f, _sceneGraphPositions[flatIndex].y};
        if (blockType == BlockType::WALL) {
            transform.position.y = 0.5f;
        }

        descriptor.drawableDescriptors.push_back(
            objects::LoaderDescriptor{
                std::string(blockTypeToPath(blockType)),
                transform,
            }
        );
        ++flatIndex;
    }
    descriptor.agentDrawableDescriptor = _agent.getDrawableDescriptor();
    return descriptor;
}

std::string GridWorld::blockTypeToPath(BlockType blockType) const {
    switch (blockType) {
    case BlockType::FLOOR:
        return RES_PATH("assets/models/floor.glb");
    case BlockType::WALL:
        return RES_PATH("assets/models/wall.glb");
    default:
        return RES_PATH("assets/models/floor.glb");
    }
}

bool GridWorld::canMoveAgent(const Position2DInt& newPosition) const {
    if (newPosition.x < 0 || newPosition.x >= _grid.shape()[0] || newPosition.y < 0 ||
        newPosition.y >= _grid.shape()[1]) {
        return false;
    }
    return _grid.get({newPosition.x, newPosition.y}) == static_cast<float>(BlockType::FLOOR);
}

void GridWorld::onKeyEvent(const vax::input::KeyEvent& keyEvent) {
    if (keyEvent.state != vax::input::KeyEvent::State::DOWN) {
        return;
    }
    MoveAction action;
    switch (keyEvent.key) {
    case vax::input::KeyCode::A:
        action = MoveAction::WEST;
        break;
    case vax::input::KeyCode::S:
        action = MoveAction::SOUTH;
        break;
    case vax::input::KeyCode::D:
        action = MoveAction::EAST;
        break;
    case vax::input::KeyCode::W:
        action = MoveAction::NORTH;
        break;
    default:
        return;
    }
    _agent.moveByOutsideAction(action);
}

void GridWorld::agentMoved() {
    auto oldPosition = std::vector<int>({_agent.getOldPosition().x, _agent.getOldPosition().y});
    auto newPosition = std::vector<int>({_agent.getPosition().x, _agent.getPosition().y});
    auto sceneGraphPosition = _sceneGraphPositions[_grid.flatIndex(newPosition)];
    _sceneGraph->moveAgent(sceneGraphPosition);
}

const Tensor& GridWorld::getGrid() const { return _grid; }

State GridWorld::resetImpl() {
    _agent.reset();
    return _agent.getPosition();
}

StepResult GridWorld::stepImpl(MoveAction action) {
    auto nextPossiblePosition = _agent.getNewPosition(action);
    if (canMoveAgent(nextPossiblePosition)) {
        _agent.allowAction(action);
        auto blockValue = _grid.get({nextPossiblePosition.x, nextPossiblePosition.y});
        if (!blockValue.has_value()) {
            return {.reward = -100.0, .done = true, .finishedWithError = true};
        }
        auto blockType = static_cast<BlockType>(blockValue.value());
        if (blockType == BlockType::TRAP) {
            return {.reward = -100.0, .done = true, .finishedWithError = false};
        }
        if (blockType == BlockType::FINISH) {
            return {.reward = 100.0, .done = true, .finishedWithError = false};
        }
        return {.reward = -1.0, .done = false, .finishedWithError = false};
    }
    return {.reward = -100.0, .done = false, .finishedWithError = false};
}

State GridWorld::getStateImpl() const {
    return _agent.getPosition();
}