#pragma once

#include "drawableModel.h"

namespace vax::rl::gw::env {
struct GridWorldDrawableDescriptor final {
    std::vector<vax::objects::DrawableModel::LoadDescriptor> drawableDescriptors;
    vax::objects::DrawableModel::LoadDescriptor agentDrawableDescriptor;
};
} // namespace vax::rl::gw::env