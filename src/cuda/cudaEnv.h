#pragma once

#include <cuda_runtime.h>
#include <vector>
#include "cudaDevice.h"
#include "logger.h"

namespace vax {

class CudaEnv {
public:
    CudaEnv() { _load(); }
    ~CudaEnv() = default;

    CudaEnv(const CudaEnv&) = delete;
    CudaEnv& operator=(const CudaEnv&) = delete;

    CudaEnv(CudaEnv&&) = default;
    CudaEnv& operator=(CudaEnv&&) = default;

    void printInfo() const;

    const std::vector<CudaDevice>& devices() const { return _devices; }

    const CudaDevice& device(int index = 0) const { return _devices[index]; }

  private:
    vax::Logger _logger = vax::Logger("CudaEnv");

    std::vector<CudaDevice> _devices;

    int _driverVersion = 0;
    int _runtimeVersion = 0;

    void _load();
};

} // namespace vax