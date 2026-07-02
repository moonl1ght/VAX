#pragma once

#include "logger.h"
#include "sceneNode.h"
#include "frameTime.h"

namespace vax::rl {
class RoverModelProxy final {
  public:
    explicit RoverModelProxy() {};
    ~RoverModelProxy() = default;

    void updateSceneNode(vax::engine::SceneNode& sceneNode);

    void linkModelNode(std::shared_ptr<vax::engine::SceneNode>& modelNode);

    void update(const vax::engine::FrameTime& frameTime);

  private:
    vax::Logger _logger = vax::Logger("RoverModelProxy");
    std::shared_ptr<vax::engine::SceneNode> _modelNode;
    vax::engine::SceneNode* _rightFrontWheel;
    vax::engine::SceneNode* _leftFrontWheel;
    vax::engine::SceneNode* _rightRearWheel;
    vax::engine::SceneNode* _leftRearWheel;
    float _wheelSpinAngle = 0.0f;
};
} // namespace vax::rl