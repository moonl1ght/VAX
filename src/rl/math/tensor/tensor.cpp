#include "tensor.h"

using namespace vax;
using namespace vax::rl::math;

Tensor Tensor::createRandom(core::RandomGenerator& generator, std::vector<int> shape, float min, float max) {
    auto tensor = Tensor(shape);
    auto distribution = std::uniform_real_distribution<float>(min, max);
    for (int i = 0; i < tensor._totalSize; i++) {
        tensor._data[i] = generator.uniformFloat(distribution);
    }
    return tensor;
}

void Tensor::_cleanup() {
    cudaFreeHost(_data);
    _freeGpuMemory();
}

void Tensor::_freeGpuMemory() {
    if (_gpuData == nullptr) {
        return;
    }
    cudaFree(_gpuData);
    _gpuData = nullptr;
}