#include "Mesh.hpp"

void Mesh::prepareForRender(VKStack *vkStack) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    if (MACOS) {
        vertexBuffer = new Buffer(
            vkStack,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vertexBuffer->fill(vertices.data());

        indexBuffer = new Buffer(
            vkStack,
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        indexBuffer->fill(indices.data());
    } else {
        Buffer stagingBuffer = Buffer(
            vkStack,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.fill(vertices.data());
        vertexBuffer = new Buffer(
            vkStack,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        stagingBuffer.copyBufferTo(vkStack, *vertexBuffer, bufferSize);

        if (!indices.empty()) {
            Buffer stagingIndexBuffer = Buffer(
                vkStack,
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            stagingIndexBuffer.fill(indices.data());
            indexBuffer = new Buffer(
                vkStack,
                indexBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            stagingIndexBuffer.copyBufferTo(vkStack, *indexBuffer, indexBufferSize);
        }
    }
}

void Mesh::cleanup(VKStack *vkStack) {
    delete vertexBuffer;
    vertexBuffer = nullptr;
    if (indexBuffer) {
        delete indexBuffer;
        indexBuffer = nullptr;
    }
}