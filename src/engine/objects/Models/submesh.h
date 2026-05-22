#pragma once

#include "luna.h"
#include "shaderUniforms.h"

namespace vax::objects {
struct Submesh final {
    uint32_t indexCount = 0;
    uint32_t firstIndex = 0;
    uint32_t vertexOffset = 0;
    uint32_t materialIndex = NO_MATERIAL_INDEX;

    void debugPrint(const vax::utils::Logger& logger) const;
};
} // namespace vax::objects