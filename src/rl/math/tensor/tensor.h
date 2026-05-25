#pragma once

#include <cuda_runtime.h>
#include <optional>
#include <vector>

#include "randomGenerator.h"

namespace vax::rl::math {
class Tensor {
  public:
    static Tensor
    createRandom(vax::core::RandomGenerator& generator, std::vector<int> shape, float min = 0.0f, float max = 1.0f);

    static Tensor createZeros(std::vector<int> shape);
    static Tensor createOnes(std::vector<int> shape);
    static Tensor createArrangeContiguous(std::vector<int> shape);

    Tensor(std::vector<int> shape)
        : _shape(shape) {
        _totalSize = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int>());
        _calculateStrides();
        cudaMallocHost((void**)&_data, _totalSize * sizeof(float));
    };

    ~Tensor() { _cleanup(); };

    Tensor(Tensor&& other) noexcept
        : _shape(other._shape)
        , _strides(other._strides)
        , _totalSize(other._totalSize)
        , _data(other._data)
        , _gpuData(other._gpuData)
        , _isCpuMemoryDirty(other._isCpuMemoryDirty) {
        other._shape = std::vector<int>();
        other._strides = std::vector<int>();
        other._totalSize = 0;
        other._data = nullptr;
        other._gpuData = nullptr;
        other._isCpuMemoryDirty = false;
    }

    Tensor& operator=(Tensor&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        other._cleanup();
        _shape = other._shape;
        _strides = other._strides;
        _totalSize = other._totalSize;
        _data = other._data;
        _gpuData = other._gpuData;
        _isCpuMemoryDirty = other._isCpuMemoryDirty;
        other._shape = std::vector<int>();
        other._strides = std::vector<int>();
        other._totalSize = 0;
        other._data = nullptr;
        other._gpuData = nullptr;
        other._isCpuMemoryDirty = false;
        return *this;
    }

    Tensor(const Tensor& other) = delete;
    Tensor& operator=(const Tensor& other) = delete;

    float* data() const { return _data; }
    float* gpuData() const { return _gpuData; }
    bool isGpuAllocated() const { return _gpuData != nullptr; }

    bool set(std::vector<int> indices, float value);
    std::optional<float> get(std::vector<int> indices) const;
    void squeeze();            // remove all dimensions of size 1
    void unsqueeze(int index); // add dimension of size 1 at the given index

    bool alignBroadcastToHigherDimensions(const std::vector<int>& otherShape);

    std::vector<int> shape() const { return _shape; }
    std::vector<int> strides() const { return _strides; }
    int totalSize() const { return _totalSize; }

    int batchSize() const;

    bool allocateGpuMemory();

    bool synchronizeHostToGpu();
    bool synchronizeGpuToHost();

  private:
    
    std::vector<int> _shape;
    std::vector<int> _strides;
    int _totalSize;
    float* _data = nullptr;
    float* _gpuData = nullptr;
    bool _isCpuMemoryDirty = true;

    void _cleanup();
    void _freeGpuMemory();
    void _calculateStrides();
    bool _checkIndices(std::vector<int> indices) const;
    int _calculateFlatIndex(std::vector<int> indices) const;
    bool _isContiguous() const;
};
} // namespace vax::rl::math