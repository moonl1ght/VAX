#pragma once

#include "drawableModel.h"
#include "modelDescriptor.h"

namespace vax::rl {
struct GridWorldDrawableDescriptor final {
    std::vector<vax::engine::ModelDescriptor> drawableDescriptors;
    vax::engine::ModelDescriptor agentDrawableDescriptor;
};
} // namespace vax::rl