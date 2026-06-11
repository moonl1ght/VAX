#pragma once

#include "gridWorldDescriptor.h"
#include "gwAgent.h"
#include "gwSceneGraph.h"
#include "inputController.h"
#include "rlMath.h"
#include "tensor.h"

namespace vax::rl::gw::env {
class GridWorld final : public vax::input::InputController::Observer {
  public:
    enum class BlockType : uint8_t {
        FLOOR = 0,
        WALL = 1,
        GOAL = 2,
        START = 3,
        AGENT = 4,
        OBSTACLE = 5,
    };

    GridWorld() {};

    ~GridWorld() {
        if (_inputController) {
            _inputController->removeObserver(this);
        }
    };

    GridWorld(const GridWorld& other) = delete;
    GridWorld& operator=(const GridWorld& other) = delete;
    GridWorld(GridWorld&& other) noexcept = delete;
    GridWorld& operator=(GridWorld&& other) noexcept = delete;

    void load();

    vax::rl::gw::env::GridWorldDrawableDescriptor getDrawableDescriptor() const;

    bool canMoveAgent(const vax::rl::math::Position2DInt& newPosition) const;

    void onMouseMove(const vax::input::MouseMoveValue& value) {};

    void onMouseWheel(float delta) {};

    void onKeyEvent(const vax::input::KeyEvent& keyEvent);

    void linkSceneGraph(GwSceneGraph* sceneGraph);

    void agentMoved();

  private:
    vax::math::Tensor _grid;
    vax::rl::gw::Agent _agent;
    std::vector<vax::rl::math::Position2DFloat> _sceneGraphPositions;

    std::string blockTypeToPath(BlockType blockType) const;
    GwSceneGraph* _sceneGraph;
};
} // namespace vax::rl::gw::env