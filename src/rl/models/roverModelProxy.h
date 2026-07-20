#pragma once

#include "logger.h"
#include "node.h"
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
    std::shared_ptr<vax::engine::Node> _modelNode;
    vax::engine::Node* _rightFrontWheel;
    vax::engine::Node* _leftFrontWheel;
    vax::engine::Node* _rightRearWheel;
    vax::engine::Node* _leftRearWheel;
    float _wheelSpinAngle = 0.0f;
};
} // namespace vax::rl