#pragma once

#include "logger.h"
#include "sceneNode.h"

namespace vax::rl::models {
class RoverModelProxy final {
  public:
    explicit RoverModelProxy() { };
    ~RoverModelProxy() = default;

    void updateSceneNode(vax::objects::SceneNode& sceneNode);

    void linkModelNode(std::shared_ptr<vax::objects::SceneNode>& modelNode);

    void update(float deltaTime);

  private:
    vax::Logger _logger = vax::Logger("RoverModelProxy");
    std::shared_ptr<vax::objects::SceneNode> _modelNode;
    vax::objects::SceneNode* _rightFrontWheel;
    vax::objects::SceneNode* _leftFrontWheel;
    vax::objects::SceneNode* _rightRearWheel;
    vax::objects::SceneNode* _leftRearWheel;
    float _wheelSpinAngle = 0.0f;
};
} // namespace vax::rl::models