#pragma once

#include "drawableModel.h"
#include "modelDescriptor.h"

namespace vax::rl::gw::env {
struct GridWorldDrawableDescriptor final {
    std::vector<vax::objects::ModelDescriptor> drawableDescriptors;
    vax::objects::ModelDescriptor agentDrawableDescriptor;
};
} // namespace vax::rl::gw::env