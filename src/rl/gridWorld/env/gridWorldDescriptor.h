#pragma once

#include "drawableModel.h"

namespace vax::rl::gw::env {
struct GridWorldDrawableDescriptor final {
    std::vector<vax::objects::DrawableModel::LoadDescriptor> drawableDescriptors;
};
} // namespace vax::rl::gw::env