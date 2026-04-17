#include "Mesh.hpp"

using namespace vax;

void Mesh::draw(
    vax::vk::Engine* vkEngine,
    VkCommandBuffer commandBuffer
) {
    if (!_isLoaded) {
        loadBuffers(vkEngine);
    }
    VkBuffer vertexBuffers[] = { vertexBuffer.vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Mesh::loadBuffers(vax::vk::Engine* vkEngine) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    if (MACOS) {
        vertexBuffer = vk::Buffer(
            *vkEngine->device,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        indexBuffer = vk::Buffer(
            *vkEngine->device,
            indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
    }
    else {
        vk::Buffer stagingBuffer = vk::Buffer(
            *vkEngine->device,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        vertexBuffer = vk::Buffer(
            *vkEngine->device,
            nullptr,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        stagingBuffer.copyBufferTo(vkEngine, vertexBuffer, bufferSize);

        if (!indices.empty()) {
            vk::Buffer stagingIndexBuffer = vk::Buffer(
                *vkEngine->device,
                indices.data(),
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            indexBuffer = vk::Buffer(
                *vkEngine->device,
                nullptr,
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );
            stagingIndexBuffer.copyBufferTo(vkEngine, indexBuffer, indexBufferSize);
        }
    }
    _isLoaded = true;
}