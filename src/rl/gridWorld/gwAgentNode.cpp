#include "gwAgentNode.h"
#include "transform.h"

using namespace vax::rl;
using namespace vax;
using namespace vax::engine;

void GWAgentNode::update(const vax::engine::FrameTime& frameTime) {
    auto spinWheel = [&](vax::engine::Node* wheel) {
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