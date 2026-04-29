#pragma once

#include "luna.h"

namespace vax::objects {
    class Submesh final {
    public:
        uint32_t indexCount;
        uint32_t firstIndex;
        uint32_t vertexOffset;
        uint32_t materialIndex;

        void debugPrint(const vax::utils::Logger& logger) const;
    };
}