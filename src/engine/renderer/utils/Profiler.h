#pragma once

namespace vax::engine {

class Profiler {
  public:
    Profiler() {};
    ~Profiler() {};

    Profiler(const Profiler& other) = delete;
    Profiler& operator=(const Profiler& other) = delete;
    Profiler(Profiler&& other) noexcept = delete;
    Profiler& operator=(Profiler&& other) noexcept = delete;
};

} // namespace vax::engine