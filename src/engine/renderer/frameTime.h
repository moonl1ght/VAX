#pragma once

#include <cstdint>

namespace vax::engine {
struct FrameTime final {
    float timestamp = 0.0f;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    uint32_t frameCount = 0;
};
} // namespace vax::engine