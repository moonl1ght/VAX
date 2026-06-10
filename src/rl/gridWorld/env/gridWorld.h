#pragma once

#include "gridWorldDescriptor.h"
#include "gwAgent.h"
#include "inputController.h"
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

    void load();

    vax::rl::gw::env::GridWorldDrawableDescriptor getDrawableDescriptor() const;

    void onMouseMove(const vax::input::MouseMoveValue& value);

    void onMouseWheel(float delta);

    void onKeyEvent(const vax::input::KeyEvent& keyEvent);

  private:
    vax::rl::math::Tensor _grid;
    vax::rl::env::gw::Agent _agent;

    std::string blockTypeToPath(BlockType blockType) const;
};
} // namespace vax::rl::gw::env