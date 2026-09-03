#include "mesh.h"

using namespace vax::vk;

template <typename VertexType>
bool vax::vk::Mesh<VertexType>::loadBuffers(const LoadMeshBuffersContext& context) {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(_indices[0]) * _indices.size();
    _stagingVertexBuffer = Buffer::allocateAndFillData(
        _device.get(),
        _name + "_vertex_buffer_staging",
        _vertices.data(),
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    );
    vertexBuffer = Buffer::allocate(
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
        _stagingIndexBuffer = vk::Buffer::allocateAndFillData(
            _device.get(),
            _name + "_index_buffer_staging",
            _indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        );
        indexBuffer = vk::Buffer::allocate(
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

template <typename VertexType> void vax::vk::Mesh<VertexType>::cleanup() {
    if (isDetached())
        _destroy();
}

template <typename VertexType> void vax::vk::Mesh<VertexType>::_destroy() {
    if (vertexBuffer.has_value()) {
        vertexBuffer.value().cleanup();
    }
    if (indexBuffer.has_value()) {
        indexBuffer.value().cleanup();
    }
    _isDetached = true;
    _id = NullId;
    _vertices.clear();
    _indices.clear();
    _isLoaded = false;
}

template <typename VertexType> void vax::vk::Mesh<VertexType>::_detach() { _isDetached = true; }

template <typename VertexType> void vax::vk::Mesh<VertexType>::cleanupStagingBuffers() {
    if (_stagingVertexBuffer.has_value()) {
        _stagingVertexBuffer->cleanup();
    }
    if (_stagingIndexBuffer.has_value()) {
        _stagingIndexBuffer->cleanup();
    }
    _stagingVertexBuffer = std::nullopt;
    _stagingIndexBuffer = std::nullopt;
}

template class vax::vk::Mesh<vax::vk::Vertex>;
template class vax::vk::Mesh<vax::vk::VertexPUV>;