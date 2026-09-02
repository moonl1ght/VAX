#pragma once

namespace vax::engine {

class FrameProfiler {
  public:
    FrameProfiler() {};
    ~FrameProfiler() {};

    FrameProfiler(const FrameProfiler& other) = delete;
    FrameProfiler& operator=(const FrameProfiler& other) = delete;
    FrameProfiler(FrameProfiler&& other) noexcept = delete;
    FrameProfiler& operator=(FrameProfiler&& other) noexcept = delete;
};

} // namespace vax::engine