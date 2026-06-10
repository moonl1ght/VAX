#include "roverModelProxy.h"
#include "transform.h"

using namespace vax::rl::models;
using namespace vax;

void RoverModelProxy::updateSceneNode(vax::objects::SceneNode& sceneNode) {
    // TODO: implement this
}

void RoverModelProxy::linkModelNode(std::shared_ptr<vax::objects::SceneNode>& modelNode) {
    _modelNode = modelNode;
    _rightFrontWheel = _modelNode->getChild("front_right_wheel_link");
    _leftFrontWheel = _modelNode->getChild("front_left_wheel_link");
    _rightRearWheel = _modelNode->getChild("rear_right_wheel_link");
    _leftRearWheel = _modelNode->getChild("rear_left_wheel_link");
}

void RoverModelProxy::update(float deltaTime) {
    auto spinWheel = [&](vax::objects::SceneNode* wheel) {
        if (!wheel) return;
        wheel->updateTransform([&](math::TransformHandle& transformHandle) {
            transformHandle.updateTransform([&](math::Transform& transform) {
                transform.updateRotationInDegrees({0.0f, -2.0f, 0.0f});
            });
        });
    };

    spinWheel(_rightFrontWheel);
    spinWheel(_leftFrontWheel);
    spinWheel(_rightRearWheel);
    spinWheel(_leftRearWheel);

    // auto moveRover = [&](vax::objects::SceneNode* rover) {
    //     if (!rover) return;
    //     rover->updateTransform([&](math::TransformHandle& transformHandle) {
    //         transformHandle.updateTransform([&](math::Transform& transform) {
    //             transform.position.z = deltaTime * 1.0f;
    //         });
    //     });
    // };

    // moveRover(_modelNode.get());
}