#include "gwAgent.h"
#include "transform.h"

using namespace vax::rl::env::gw;
using namespace vax;

Agent::Agent() {
    _position = {0, 0};
}

Agent::~Agent() {
}

vax::objects::DrawableModel::LoadDescriptor Agent::getDrawableDescriptor() const {
    vax::math::Transform transform = vax::math::Transform();
    transform.position.y = 0.31f;
    return {
        RES_PATH("assets/models/rover_s1_v1.glb"),
        transform,
    };
}