#include "mesh.h"

using namespace vax;

// bool vax::objects::Mesh::draw(
//     VkCommandBuffer commandBuffer
// ) const {
//     if (!_isLoaded || !vertexBuffer.has_value() || !indexBuffer.has_value()) {
//         _logger.warning("Mesh is not loaded, skipping draw");
//         return false;
//     }
//     VkBuffer vertexBuffers[] = { vertexBuffer.value().vkBuffer() };
//     VkDeviceSize offsets[] = { 0 };
//     vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//     vkCmdBindIndexBuffer(commandBuffer, indexBuffer.value().vkBuffer(), 0, VK_INDEX_TYPE_UINT32);
//     vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
//     return true;
// }

// void vax::objects::Mesh::forceDraw(
//     vax::vk::QueueManager& queueManager,
//     vax::vk::CommandManager& commandManager,
//     VkCommandBuffer commandBuffer
// ) {
//     if (!_isLoaded || !vertexBuffer.has_value() || !indexBuffer.has_value()) {
//         loadBuffers(queueManager, commandManager);
//     }
//     VkBuffer vertexBuffers[] = { vertexBuffer.value().vkBuffer() };
//     VkDeviceSize offsets[] = { 0 };
//     vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//     vkCmdBindIndexBuffer(commandBuffer, indexBuffer.value().vkBuffer(), 0, VK_INDEX_TYPE_UINT32);
//     vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
// }

bool vax::objects::Mesh::loadBuffers(
    vax::vk::CommandBuffer& commandBuffer
) {
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(_indices[0]) * _indices.size();
    if (MACOS) {
        vertexBuffer.emplace(vk::Buffer::allocateAndFillData(
            _device.get(),
            _name + "_vertex_buffer",
            _vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value());

        indexBuffer.emplace(vk::Buffer::allocateAndFillData(
            _device.get(),
            _name + "_index_buffer",
            _indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value());
    } else {
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
        _stagingVertexBuffer->copyBufferCommand(
            commandBuffer, *vertexBuffer, bufferSize
        );

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
            _stagingIndexBuffer->copyBufferCommand(
                commandBuffer, *indexBuffer, indexBufferSize
            );
        }
    }
    _isLoaded = true;
    return true;
}

void vax::objects::Mesh::cleanup() {
    if (isDetached()) _destroy();
}

void vax::objects::Mesh::_destroy() {
    if (vertexBuffer.has_value()) {
        vertexBuffer.value().cleanup();
    }
    if (indexBuffer.has_value()) {
        indexBuffer.value().cleanup();
    }
    _isDetached = true;
    _indices.clear();
    _isLoaded = false;
    _id = vax::NullId;
    _vertices.clear();
    _indices.clear();
    _isLoaded = false;
}

void vax::objects::Mesh::_detach() {
    _isDetached = true;
}

void vax::objects::Mesh::cleanupStagingBuffers() {
    if (_stagingVertexBuffer.has_value()) {
        _stagingVertexBuffer->cleanup();
    }
    if (_stagingIndexBuffer.has_value()) {
        _stagingIndexBuffer->cleanup();
    }
    _stagingVertexBuffer = std::nullopt;
    _stagingIndexBuffer = std::nullopt;
}