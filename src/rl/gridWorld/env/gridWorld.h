#pragma once

#include "gridWorldDescriptor.h"
#include "gwAgent.h"
#include "gwSceneGraph.h"
#include "gwenv.h"
#include "inputController.h"
#include "qlConfig.h"
#include "rlMath.h"
#include "rlenv.h"
#include "tensor.h"

namespace vax::rl::gw::env {
class GridWorld final : public vax::input::InputController::Observer,
                        public vax::rl::Environment<GridWorld, State, MoveAction> {
  public:
    enum class BlockType : uint8_t {
        FLOOR = 0,
        WALL = 1,
        FINISH = 2,
        START = 3,
        TRAP = 4,
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

    const vax::math::Tensor& getGrid() const;

    int resetImpl();

    StepResult stepImpl(MoveAction action);

  private:
    vax::rl::ql::QLearningConfig _qlConfig;
    vax::math::Tensor _grid;
    vax::rl::gw::Agent _agent = vax::rl::gw::Agent(_qlConfig);
    std::vector<vax::rl::math::Position2DFloat> _sceneGraphPositions;

    std::string blockTypeToPath(BlockType blockType) const;
    GwSceneGraph* _sceneGraph;
};
} // namespace vax::rl::gw::env