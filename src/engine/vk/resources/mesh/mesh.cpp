#include "mesh.h"
#include "vertex.h"
#include <cstdint>

using namespace vax::vk;

template <typename VertexType> void vax::vk::MeshObject<VertexType>::setVertices(std::vector<VertexType> vertices) {
    if (!_locked) {
        _vertices = std::move(vertices);
    }
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::addVertex(const VertexType& vertex) {
    if (!_locked) {
        _vertices.push_back(vertex);
    }
}

template<typename VertexType> void vax::vk::MeshObject<VertexType>::setIndices(std::vector<uint32_t> indices) {
    if (!_locked) {
        _indices = std::move(indices);
    }
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::addIndex(uint32_t index) {
    if (!_locked) {
        _indices.push_back(index);
    }
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::lock() {
    _locked = true;
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::unlock() {
    _locked = false;
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::bindToCommandBuffer(CommandBuffer* commandBuffer) {
    if (_bound) {
        return;
    }
}

template <typename VertexType> bool vax::vk::MeshObject<VertexType>::bindBuffers() {
    if (_bound) {
        return true;
    }
    return false;
}

template <typename VertexType>
bool vax::vk::MeshObject<VertexType>::loadBuffers(const LoadMeshBuffersContext& context) {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(_indices[0]) * _indices.size();
    _stagingVertexBuffer = Buffer<VertexType>::allocateAndFillData(
        _device.get(),
        _name + "_vertex_buffer_staging",
        _vertices.data(),
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    );
    vertexBuffer = Buffer<VertexType>::allocate(
        _device.get(),
        _name + "_vertex_buffer",
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    );
    if (!_stagingVertexBuffer.has_value() && !vertexBuffer.has_value()) {
        return false;
    }
    _stagingVertexBuffer->copyBufferCommand(*context.commandBuffer, *vertexBuffer, bufferSize);

    if (!_indices.empty()) {
        _stagingIndexBuffer = Buffer<uint32_t>::allocateAndFillData(
            _device.get(),
            _name + "_index_buffer_staging",
            _indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        );
        indexBuffer = Buffer<uint32_t>::allocate(
            _device.get(),
            _name + "_index_buffer",
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY
        );
        if (!_stagingIndexBuffer.has_value() && !indexBuffer.has_value()) {
            return false;
        }
        _stagingIndexBuffer->copyBufferCommand(*context.commandBuffer, *indexBuffer, indexBufferSize);
    }
    _isLoaded = true;
    return true;
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::_destroy() {
    if (vertexBuffer.has_value()) {
        vertexBuffer.value().cleanup();
    }
    if (indexBuffer.has_value()) {
        indexBuffer.value().cleanup();
    }
    _id = NullId;
    _vertices.clear();
    _indices.clear();
    _isLoaded = false;
}

template <typename VertexType> void vax::vk::MeshObject<VertexType>::cleanupStagingBuffers() {
    if (_stagingVertexBuffer.has_value()) {
        _stagingVertexBuffer->cleanup();
    }
    if (_stagingIndexBuffer.has_value()) {
        _stagingIndexBuffer->cleanup();
    }
    _stagingVertexBuffer = std::nullopt;
    _stagingIndexBuffer = std::nullopt;
}

template class vax::vk::MeshObject<vax::vk::Vertex>;
template class vax::vk::MeshObject<vax::vk::VertexPUV>;