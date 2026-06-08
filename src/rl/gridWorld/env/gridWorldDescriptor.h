#pragma once

#include "drawableModel.h"
#include "loaderDescriptor.h"

namespace vax::rl::gw::env {
struct GridWorldDrawableDescriptor final {
    std::vector<vax::objects::LoaderDescriptor> drawableDescriptors;
    vax::objects::LoaderDescriptor agentDrawableDescriptor;
};
} // namespace vax::rl::gw::env