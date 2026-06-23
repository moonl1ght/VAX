#include "mesh.h"

using namespace vax;

template <typename VertexType> bool vax::objects::Mesh<VertexType>::loadBuffers(vax::vk::CommandBuffer& commandBuffer) {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(_indices[0]) * _indices.size();
    _stagingVertexBuffer = vk::Buffer::allocateAndFillData(
        _device.get(),
        _name + "_vertex_buffer_staging",
        _vertices.data(),
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    vertexBuffer = vk::Buffer::allocate(
        _device.get(),
        _name + "_vertex_buffer",
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    if (!_stagingVertexBuffer.has_value() && !vertexBuffer.has_value()) {
        return false;
    }
    _stagingVertexBuffer->copyBufferCommand(commandBuffer, *vertexBuffer, bufferSize);

    if (!_indices.empty()) {
        _stagingIndexBuffer = vk::Buffer::allocateAndFillData(
            _device.get(),
            _name + "_index_buffer_staging",
            _indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        indexBuffer = vk::Buffer::allocate(
            _device.get(),
            _name + "_index_buffer",
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        if (!_stagingIndexBuffer.has_value() && !indexBuffer.has_value()) {
            return false;
        }
        _stagingIndexBuffer->copyBufferCommand(commandBuffer, *indexBuffer, indexBufferSize);
    }
    if (_instancesCount > 1) {
        instancesBuffer = vk::Buffer::allocate(
            _device.get(),
            _name + "_instance_buffer",
            sizeof(InstanceData) * _instancesCount,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        if (instancesBuffer.has_value()) {
            instancesBuffer->map();
        }
    }
    _isLoaded = true;
    return true;
}

template <typename VertexType> void vax::objects::Mesh<VertexType>::cleanup() {
    if (isDetached())
        _destroy();
}

template <typename VertexType> void vax::objects::Mesh<VertexType>::_destroy() {
    if (vertexBuffer.has_value()) {
        vertexBuffer.value().cleanup();
    }
    if (indexBuffer.has_value()) {
        indexBuffer.value().cleanup();
    }
    if (instancesBuffer.has_value()) {
        instancesBuffer.value().cleanup();
    }
    _isDetached = true;
    _id = vax::NullId;
    _vertices.clear();
    _indices.clear();
    _isLoaded = false;
}

template <typename VertexType> void vax::objects::Mesh<VertexType>::_detach() { _isDetached = true; }

template <typename VertexType> void vax::objects::Mesh<VertexType>::cleanupStagingBuffers() {
    if (_stagingVertexBuffer.has_value()) {
        _stagingVertexBuffer->cleanup();
    }
    if (_stagingIndexBuffer.has_value()) {
        _stagingIndexBuffer->cleanup();
    }
    _stagingVertexBuffer = std::nullopt;
    _stagingIndexBuffer = std::nullopt;
}

template class vax::objects::Mesh<vax::objects::Vertex>;
template class vax::objects::Mesh<vax::objects::VertexPUV>;