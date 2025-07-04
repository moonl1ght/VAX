#include "Mesh.hpp"

void Mesh::draw(VKStack* vkStack) {
    if (!_isLoaded) {
        loadBuffers(vkStack);
    }
}

void Mesh::loadBuffers(VKStack* vkStack) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    if (MACOS) {
        vertexBuffer = Buffer(
            vkStack,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        indexBuffer = Buffer(
            vkStack,
            indices.data(),
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
    }
    else {
        Buffer stagingBuffer = Buffer(
            vkStack,
            vertices.data(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        vertexBuffer = Buffer(
            vkStack,
            nullptr,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        stagingBuffer.copyBufferTo(vkStack, vertexBuffer, bufferSize);

        if (!indices.empty()) {
            Buffer stagingIndexBuffer = Buffer(
                vkStack,
                indices.data(),
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            indexBuffer = Buffer(
                vkStack,
                nullptr,
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );
            stagingIndexBuffer.copyBufferTo(vkStack, indexBuffer, indexBufferSize);
        }
    }
    _isLoaded = true;
}