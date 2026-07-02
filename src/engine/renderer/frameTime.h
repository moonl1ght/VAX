#pragma once

#include <cstdint>

namespace vax::engine {
struct FrameTime final {
    float _timestamp = 0.0f;
    float _deltaTime = 0.0f;
    float _fps = 0.0f;
    uint32_t _frameCount = 0;
};
} // namespace vax::engine