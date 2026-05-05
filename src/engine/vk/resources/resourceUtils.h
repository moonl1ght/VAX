#pragma once

#include <cstdint>
#include <limits>

namespace vax {
    constexpr uint32_t NullId = std::numeric_limits<uint32_t>::max();

    using MeshId = uint32_t;
    constexpr MeshId NullMeshId = std::numeric_limits<MeshId>::max();
    using BufferId = uint32_t;
    constexpr BufferId NullBufferId = std::numeric_limits<BufferId>::max();
    using TextureId = uint32_t;
    constexpr TextureId NullTextureId = std::numeric_limits<TextureId>::max();
    using SamplerId = uint32_t;
    constexpr SamplerId NullSamplerId = std::numeric_limits<SamplerId>::max();
}