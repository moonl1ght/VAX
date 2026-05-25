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

Tensor Tensor::createZeros(std::vector<int> shape) {
    auto tensor = Tensor(shape);
    for (int i = 0; i < tensor._totalSize; i++) {
        tensor._data[i] = 0.0f;
    }
    return tensor;
}

Tensor Tensor::createOnes(std::vector<int> shape) {
    auto tensor = Tensor(shape);
    for (int i = 0; i < tensor._totalSize; i++) {
        tensor._data[i] = 1.0f;
    }
    return tensor;
}

Tensor Tensor::createArrangeContiguous(std::vector<int> shape) {
    auto tensor = Tensor(shape);
    for (int i = 0; i < tensor._totalSize; i++) {
        tensor._data[i] = static_cast<float>(i);
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

bool Tensor::allocateGpuMemory() {
    if (_gpuData != nullptr) {
        return false;
    }
    cudaMalloc((void**)&_gpuData, _totalSize * sizeof(float));
    return true;
}

bool Tensor::synchronizeHostToGpu() {
    if (_data == nullptr) {
        return false;
    }
    cudaMemcpy(_gpuData, _data, _totalSize * sizeof(float), cudaMemcpyHostToDevice);
    _isCpuMemoryDirty = false;
    return true;
}

bool Tensor::synchronizeGpuToHost() {
    if (_gpuData == nullptr) {
        return false;
    }
    cudaMemcpy(_data, _gpuData, _totalSize * sizeof(float), cudaMemcpyDeviceToHost);
    _isCpuMemoryDirty = false;
    return true;
}

bool Tensor::set(std::vector<int> indices, float value) {
    if (!_checkIndices(indices)) {
        return false;
    }
    _isCpuMemoryDirty = true;
    _data[_calculateFlatIndex(indices)] = value;
    return true;
}

std::optional<float> Tensor::get(std::vector<int> indices) const {
    if (!_checkIndices(indices)) {
        return std::nullopt;
    }
    return _data[_calculateFlatIndex(indices)];
}

bool Tensor::_checkIndices(std::vector<int> indices) const {
    if (indices.size() != _shape.size()) {
        return false;
    }
    for (std::vector<int>::size_type i = 0; i < indices.size(); i++) {
        if (indices[i] < 0 || indices[i] >= _shape[i]) {
            return false;
        }
    }
    return true;
}

bool Tensor::_isContiguous() const {
    int expected_stride = 1;
    for (int i = _shape.size() - 1; i >= 0; --i) {
        if (_shape[i] > 1 && _strides[i] != expected_stride) {
            return false;
        }
        expected_stride *= _shape[i];
    }
    return true;
}

int Tensor::_calculateFlatIndex(std::vector<int> indices) const {
    int index = 0;
    for (std::vector<int>::size_type i = 0; i < indices.size() - 1; i++) {
        index += indices[i] * _strides[i];
    }
    index += indices[indices.size() - 1];
    return index;
}

void Tensor::_calculateStrides() {
    _strides = std::vector<int>(_shape.size(), 1);
    for (int i = _strides.size() - 2; i >= 0; i--) {
        _strides[i] = _strides[i + 1] * _shape[i + 1];
    }
}

void Tensor::squeeze() {
    std::erase(_shape, 1);
    _calculateStrides();
}

void Tensor::unsqueeze(int index) {
    _shape.insert(_shape.begin() + index, 1);
    _calculateStrides();
}

int Tensor::batchSize() const {
    if (_shape.size() < 2) {
        return 1;
    }
    return std::accumulate(_shape.begin(), _shape.end() - 2, 1, std::multiplies<int>());
}

bool Tensor::alignBroadcastToHigherDimensions(const std::vector<int>& otherShape) {
    if (otherShape.size() < _shape.size()) {
        return false;
    }
    while (_shape.size() < otherShape.size()) {
        unsqueeze(0);
    }
    return true;
}