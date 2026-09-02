#pragma once

#include "cudaEnv.h"
#include <string>

namespace vax {
class SystemInfo final {
  public:
    struct MemoryStats { // in MB
        int total;
        int available;
        int used;
        int free;
        float usagePercentage;
    };

    struct GPUStats {
        std::string name;
        MemoryStats memoryStats;
    };

    SystemInfo() {
        _cudaEnv = CudaEnv();
    };

    ~SystemInfo() = default;

    MemoryStats getRAMStats() const;

    GPUStats getGPUStats() const;

  private:
    CudaEnv _cudaEnv;
};
} // namespace vax