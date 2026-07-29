#include "cudaEnv.h"

using namespace vax;

std::string version(int packed) { return std::to_string(packed / 1000) + "." + std::to_string((packed % 1000) / 10); }

void CudaEnv::printInfo() const {
    _logger.info("===== CUDA =====");
    _logger.info("Driver version:               ", version(_driverVersion));
    _logger.info("Runtime version:              ", version(_runtimeVersion));
    _logger.info("Device count:                 ", _devices.size());

    for (const auto& device : _devices) {
        device.printGpuInfo();
    }

    _logger.info("================");
}

void CudaEnv::_load() {
    int deviceCount = 0;
    auto countResult = cudaGetDeviceCount(&deviceCount);
    if (countResult != cudaSuccess) {
        return;
    }
    if (deviceCount == 0) {
        _logger.warning("No CUDA capable devices found");
        return;
    }

    cudaDriverGetVersion(&_driverVersion);
    cudaRuntimeGetVersion(&_runtimeVersion);

    for (int i = 0; i < deviceCount; ++i) {
        auto device = CudaDevice(i);
        if (device.deviceExists()) {
            _devices.push_back(device);
        }
    }
}