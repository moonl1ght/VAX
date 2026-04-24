#pragma once

#include <cstdint>
#include <limits>

namespace vax {
    constexpr uint32_t NullId = std::numeric_limits<uint32_t>::max();

    using MeshId = uint32_t;
    using BufferId = uint32_t;
}