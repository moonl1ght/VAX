#pragma once

#include "sceneNode.h"

namespace vax::rl::models {
class RoverModelProxy final {
  public:
    explicit RoverModelProxy() { };
    ~RoverModelProxy() = default;

    void updateSceneNode(vax::objects::SceneNode& sceneNode);
};
} // namespace vax::rl::models