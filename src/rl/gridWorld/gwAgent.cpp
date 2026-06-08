#include "gwAgent.h"
#include "transform.h"

using namespace vax::rl::env::gw;
using namespace vax;

Agent::Agent() {
    _position = {0, 0};
}

Agent::~Agent() {
}

vax::objects::LoaderDescriptor Agent::getDrawableDescriptor() const {
    vax::math::Transform transform = vax::math::Transform();
    transform.position.y = 0.31f;
    return {
        RES_PATH("assets/models/rover/rover.urdf"),
        transform,
    };
}