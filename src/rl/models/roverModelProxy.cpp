#include "roverModelProxy.h"
#include "transform.h"

using namespace vax::rl;
using namespace vax;
using namespace vax::engine;

void RoverModelProxy::updateSceneNode(vax::engine::SceneNode& sceneNode) {
    // TODO: implement this
}

void RoverModelProxy::linkModelNode(std::shared_ptr<vax::engine::SceneNode>& modelNode) {
    _modelNode = modelNode;
    _rightFrontWheel = _modelNode->getChild("front_right_wheel_link");
    _leftFrontWheel = _modelNode->getChild("front_left_wheel_link");
    _rightRearWheel = _modelNode->getChild("rear_right_wheel_link");
    _leftRearWheel = _modelNode->getChild("rear_left_wheel_link");
}

void RoverModelProxy::update(const vax::engine::FrameTime& frameTime) {
    auto spinWheel = [&](vax::engine::SceneNode* wheel) {
        if (!wheel)
            return;
        wheel->updateTransform([&](math::TransformHandle& transformHandle) {
            transformHandle.updateTransform([&](math::Transform& transform) {
                transform.updateRotationInDegrees({0.0f, frameTime.timestamp * -100.0f, 0.0f});
            });
        });
    };

    spinWheel(_rightFrontWheel);
    spinWheel(_leftFrontWheel);
    spinWheel(_rightRearWheel);
    spinWheel(_leftRearWheel);
}