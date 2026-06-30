#pragma once

#include "gridWorldDescriptor.h"
#include "gwAgent.h"
#include "gwSceneGraph.h"
#include "gwenv.h"
#include "inputController.h"
#include "logger.h"
#include "qlConfig.h"
#include "rlMath.h"
#include "rlenv.h"
#include "tensor.h"

namespace vax::rl::gw::env {
class GridWorld final : public vax::InputController::Observer,
                        public vax::rl::Environment<GridWorld, State, MoveAction> {
  public:
    enum class BlockType : uint8_t {
        FLOOR = 0,
        WALL = 1,
        FINISH = 2,
        START = 3,
        TRAP = 4,
    };

    explicit GridWorld(vax::rl::ql::QLearningConfig qlConfig)
        : _qlConfig(qlConfig) {};

    ~GridWorld() {
        if (_inputController) {
            _inputController->removeObserver(this);
        }
    };

    GridWorld(const GridWorld& other) = delete;
    GridWorld& operator=(const GridWorld& other) = delete;
    GridWorld(GridWorld&& other) noexcept = delete;
    GridWorld& operator=(GridWorld&& other) noexcept = delete;

    void createRandomGrid();

    void save(const std::string& folderPath);

    bool load(const std::string& folderPath);

    vax::rl::gw::env::GridWorldDrawableDescriptor getDrawableDescriptor() const;

    bool canMoveAgent(const vax::rl::math::Position2DInt& newPosition) const;

    void onMouseMove(const vax::MouseMoveValue& value) {};

    void onMouseWheel(float delta) {};

    void onKeyEvent(const vax::KeyEvent& keyEvent);

    void linkSceneGraph(GwSceneGraph* sceneGraph);

    void agentMoved();

    const vax::math::Tensor& getGrid() const;

    State resetImpl();

    State getStateImpl() const;

    vax::rl::StepResult stepImpl(MoveAction action);

    const std::string& nameImpl() const { return _name; }

    void setEvalModeImpl(vax::rl::EvalMode evalMode);

    vax::rl::gw::Agent& getAgent() { return _agent; }

    void setFsLogger(std::shared_ptr<vax::FsLogger> fsLogger);

  private:
    vax::Logger _logger = vax::Logger("GridWorld");
    vax::rl::ql::QLearningConfig _qlConfig;
    std::string _name = "GridWorld";
    vax::math::Tensor _grid;
    vax::rl::gw::Agent _agent = vax::rl::gw::Agent(_qlConfig);
    std::vector<vax::rl::math::Position2DFloat> _sceneGraphPositions;

    std::string blockTypeToPath(BlockType blockType) const;
    GwSceneGraph* _sceneGraph;
    vax::rl::EvalMode _evalMode = vax::rl::EvalMode::EVALUATION;
};
} // namespace vax::rl::gw::env