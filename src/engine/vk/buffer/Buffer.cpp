#include "Buffer.hpp"

using namespace vax::vk;

Buffer::Buffer(
    vax::vk::Engine* vkEngine,
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
)
    : _device(vkEngine->device->vkDevice), size(size)
{
    load(vkEngine, usage, properties);
    fill(data);
}

Buffer::~Buffer() {
    cleanup();
}

void Buffer::cleanup() {
    if (_device == VK_NULL_HANDLE) {
        return;
    }
    if (vkBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(_device, vkBuffer, nullptr);
        vkBuffer = VK_NULL_HANDLE;
    }
    if (vkBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_device, vkBufferMemory, nullptr);
        vkBufferMemory = VK_NULL_HANDLE;
    }
}

void Buffer::reload(
    vax::vk::Engine* vkEngine,
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    cleanup();
    this->_device = vkEngine->device->vkDevice;
    this->size = size;
    load(vkEngine, usage, properties);
    fill(data);
}

void Buffer::load(
    vax::vk::Engine* vkEngine,
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    if (isLoaded()) {
        return;
    }

    this->_device = vkEngine->device->vkDevice;
    this->size = size;
    load(vkEngine, usage, properties);
    fill(data);
}

void Buffer::fill(const void* fillData) {
    if (isEmpty() || !isLoaded() || fillData == nullptr) {
        return;
    }

    void* data;
    vkMapMemory(_device, vkBufferMemory, 0, size, 0, &data);
    memcpy(data, fillData, (size_t)size);
    vkUnmapMemory(_device, vkBufferMemory);
}

void Buffer::copyBufferTo(vax::vk::Engine* vkEngine, Buffer& dstBuffer, VkDeviceSize size) const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkEngine->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vkEngine->device->vkDevice, &allocInfo, &commandBuffer);

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

    vkQueueSubmit(vkEngine->queueManager->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkEngine->queueManager->graphicsQueue);

    vkFreeCommandBuffers(vkEngine->device->vkDevice, vkEngine->commandPool, 1, &commandBuffer);
}

bool Buffer::isEmpty() const {
    return size == 0;
}

bool Buffer::isLoaded() const {
    return vkBuffer != VK_NULL_HANDLE && vkBufferMemory != VK_NULL_HANDLE;
}

void Buffer::load(
    vax::vk::Engine* vkEngine,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    if (isLoaded()) {
        return;
    }

    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkEngine->device->vkDevice, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
        Logger::getInstance().error("failed to create buffer!");
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkEngine->device->vkDevice, vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = utils::findMemoryType(vkEngine->device->vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkEngine->device->vkDevice, &allocInfo, nullptr, &vkBufferMemory) != VK_SUCCESS) {
        Logger::getInstance().error("failed to allocate buffer memory!");
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vkEngine->device->vkDevice, vkBuffer, vkBufferMemory, 0);
}