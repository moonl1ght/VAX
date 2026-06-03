#pragma once

#include "tensor.h"
#include "gridWorldDescriptor.h"
#include "gwAgent.h"

namespace vax::rl::gw::env {
class GridWorld final {
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
    ~GridWorld() {};

    void load();

    vax::rl::gw::env::GridWorldDrawableDescriptor getDrawableDescriptor() const;

  private:
    vax::rl::math::Tensor _grid;
    vax::rl::env::gw::Agent _agent;

    std::string blockTypeToPath(BlockType blockType) const;
};
} // namespace vax::rl::gw::env