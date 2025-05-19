#include "Mesh.hpp"

void Mesh::prepareForRender(VKStack *vkStack) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    if (MACOS) {
        vertexBuffer = new Buffer(
            vkStack,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vertexBuffer->fill(vertices.data());
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
    }
}

void Mesh::cleanup(VKStack *vkStack) {
    delete vertexBuffer;
    vertexBuffer = nullptr;
}