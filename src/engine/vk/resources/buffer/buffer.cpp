#include "buffer.h"
#include "vkEngine.h"
#include "commandManager.h"
#include "queueManager.h"

using namespace vax::vk;

using namespace vax;

std::optional<Buffer> Buffer::allocateAndFillData(
    const vax::vk::Device& device,
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    auto buffer = Buffer(device);
    buffer._size = size;
    if (!buffer._allocate(usage, properties)) return std::nullopt;
    if (!buffer.fill(data)) return std::nullopt;
    return buffer;
}

std::optional<Buffer> Buffer::allocate(
    const vax::vk::Device& device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    auto buffer = Buffer(device);
    buffer._size = size;
    if (!buffer._allocate(usage, properties)) return std::nullopt;
    return buffer;
}

void Buffer::cleanup() {
    if (isDetached()) return;
    _destroy();
}

void Buffer::_detach() {
    _isDetached = true;
    _id = NullId;
}

void Buffer::_destroy() {
    if (_vkBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(_device.get().vkDevice, _vkBuffer, nullptr);
        _vkBuffer = VK_NULL_HANDLE;
    }
    if (_vkBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_device.get().vkDevice, _vkBufferMemory, nullptr);
        _vkBufferMemory = VK_NULL_HANDLE;
    }
    _isDetached = true;
    _id = NullId;
}

void Buffer::bind(void* data) {
    if (isEmpty() || !isAllocated()) {
        return;
    }

    vkMapMemory(
        _device.get().vkDevice,
        _vkBufferMemory,
        0,
        _size,
        0,
        &data
    );
}

bool Buffer::reload(
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    cleanup();
    _size = size;
    if (!_allocate(usage, properties)) return false;
    if (!fill(data)) return false;
    return true;
}

bool Buffer::load(
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    if (isAllocated())
    {
        return false;
    }

    _size = size;
    if (!_allocate(usage, properties)) return false;
    if (!fill(data)) return false;
    return true;
}

bool Buffer::fill(const void* fillData) {
    if (isEmpty() || !isAllocated() || fillData == nullptr)
    {
        return false;
    }

    void* data;
    vkMapMemory(_device.get().vkDevice, _vkBufferMemory, 0, _size, 0, &data);
    memcpy(data, fillData, (size_t)_size);
    vkUnmapMemory(_device.get().vkDevice, _vkBufferMemory);
    return true;
}

bool vax::vk::Buffer::copyBufferToSync(
    const QueueManager& queueManager,
    CommandManager& commandManager,
    Buffer& dstBuffer,
    VkDeviceSize size
) const {
    auto commandBuffer = commandManager.createSingleTimeCommandBuffer();
    commandBuffer.begin();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer.vkCommandBuffer, _vkBuffer, dstBuffer._vkBuffer, 1, &copyRegion);

    commandBuffer.end();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.vkCommandBuffer;

    vkQueueSubmit(queueManager.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queueManager.graphicsQueue);
    return true;
}

bool Buffer::isEmpty() const {
    return _size == 0;
}

bool Buffer::isAllocated() const {
    return _vkBuffer != VK_NULL_HANDLE && _vkBufferMemory != VK_NULL_HANDLE;
}

bool Buffer::_allocate(
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    if (isAllocated()) {
        return false;
    }

    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(_device.get().vkDevice, &bufferInfo, nullptr, &_vkBuffer) != VK_SUCCESS) {
        _logger.error("failed to create buffer!");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device.get().vkDevice, _vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = utils::findMemoryType(
        _device.get().vkPhysicalDevice, memRequirements.memoryTypeBits, properties
    );

    if (vkAllocateMemory(_device.get().vkDevice, &allocInfo, nullptr, &_vkBufferMemory) != VK_SUCCESS) {
        _logger.error("failed to allocate buffer memory!");
        return false;
    }

    vkBindBufferMemory(_device.get().vkDevice, _vkBuffer, _vkBufferMemory, 0);
    return true;
}