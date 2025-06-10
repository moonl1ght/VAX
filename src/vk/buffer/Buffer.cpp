#include "Buffer.hpp"

Buffer::Buffer(
    VKStack* vkStack,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
)
    : device(vkStack->device->vkDevice), size(size)
{
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkStack->device->vkDevice, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkStack->device->vkDevice, vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VKUtils::findMemoryType(vkStack->device->vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkStack->device->vkDevice, &allocInfo, nullptr, &vkBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vkStack->device->vkDevice, vkBuffer, vkBufferMemory, 0);
}

Buffer::~Buffer() {
    vkDestroyBuffer(device, vkBuffer, nullptr);
    vkFreeMemory(device, vkBufferMemory, nullptr);
}

void Buffer::fill(const void* fillData) {
    void* data;
    vkMapMemory(device, vkBufferMemory, 0, size, 0, &data);
    memcpy(data, fillData, (size_t)size);
    vkUnmapMemory(device, vkBufferMemory);
}

void Buffer::copyBufferTo(VKStack* vkStack, Buffer& dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkStack->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vkStack->device->vkDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, vkBuffer, dstBuffer.vkBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(vkStack->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkStack->graphicsQueue);

    vkFreeCommandBuffers(vkStack->device->vkDevice, vkStack->commandPool, 1, &commandBuffer);
}