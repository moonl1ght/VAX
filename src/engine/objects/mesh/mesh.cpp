#include "mesh.h"

using namespace vax;

bool vax::objects::Mesh::draw(
    VkCommandBuffer commandBuffer
) const {
    if (!_isLoaded || !vertexBuffer.has_value() || !indexBuffer.has_value()) {
        _logger.warning("Mesh is not loaded, skipping draw");
        return false;
    }
    VkBuffer vertexBuffers[] = { vertexBuffer.value().getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.value().getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    return true;
}

void vax::objects::Mesh::forceDraw(
    vax::vk::Engine* vkEngine,
    VkCommandBuffer commandBuffer
) {
    if (!_isLoaded || !vertexBuffer.has_value() || !indexBuffer.has_value()) {
        loadBuffers(vkEngine);
    }
    VkBuffer vertexBuffers[] = { vertexBuffer.value().getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.value().getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

bool vax::objects::Mesh::loadBuffers(vax::vk::Engine* vkEngine) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    if (MACOS) {
        vertexBuffer.emplace(vk::Buffer::allocateAndFillData(
            *vkEngine->device,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value());

        indexBuffer.emplace(vk::Buffer::allocateAndFillData(
            *vkEngine->device,
            indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value());
    } else {
        auto stagingBuffer = vk::Buffer::allocateAndFillData(
            *vkEngine->device,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ).value();
        vertexBuffer.emplace(vk::Buffer::allocate(
            *vkEngine->device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        ).value());
        stagingBuffer.copyBufferTo(*vkEngine->queueManager, *vkEngine->commandManager, vertexBuffer.value(), bufferSize);

        if (!indices.empty()) {
            auto stagingIndexBuffer = vk::Buffer::allocateAndFillData(
                *vkEngine->device,
                indices.data(),
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            ).value();
            indexBuffer.emplace(vk::Buffer::allocate(
                *vkEngine->device,
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            ).value());
            stagingIndexBuffer.copyBufferTo(*vkEngine->queueManager, *vkEngine->commandManager, indexBuffer.value(), indexBufferSize);
        }
    }
    _isLoaded = true;
    return true;
}