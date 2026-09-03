#include "buffer.h"
#include "commandBuffer.h"

using namespace vax::vk;

using namespace vax;

std::optional<Buffer> Buffer::allocateAndFillData(
    const vax::vk::Device& device,
    std::string name,
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags
) {
    auto buffer = Buffer(device);
    buffer._name = name;
    buffer._size = size;
    if (!buffer._allocate(usage, memoryUsage, flags))
        return std::nullopt;
    if (!buffer.fill(data))
        return std::nullopt;
    return buffer;
}

std::optional<Buffer> Buffer::allocate(
    const vax::vk::Device& device,
    std::string name,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags
) {
    auto buffer = Buffer(device);
    buffer._name = name;
    buffer._size = size;
    if (!buffer._allocate(usage, memoryUsage, flags))
        return std::nullopt;
    return buffer;
}

void Buffer::cleanup() {
    if (isDetached())
        _destroy();
}

void Buffer::_detach() {
    _isDetached = true;
    _id = NullId;
}

void Buffer::_destroy() {
    unmap();
    if (_vkBuffer != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE) {
        vmaDestroyBuffer(_device.get().allocator, _vkBuffer, _allocation);
        _vkBuffer = VK_NULL_HANDLE;
        _allocation = VK_NULL_HANDLE;
    }
    _mappedMemory = nullptr;
    _isMapped = false;
    _isDetached = true;
    _id = NullId;
}

bool Buffer::reload(
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags
) {
    cleanup();
    _size = size;
    if (!_allocate(usage, memoryUsage, flags))
        return false;
    if (!fill(data))
        return false;
    return true;
}

bool Buffer::load(
    const void* data,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags
) {
    if (isAllocated()) {
        return false;
    }

    _size = size;
    if (!_allocate(usage, memoryUsage, flags))
        return false;
    if (!fill(data))
        return false;
    return true;
}

bool Buffer::fill(const void* fillData) {
    if (isEmpty() || !isAllocated() || fillData == nullptr) {
        return false;
    }

    map();
    memcpy(_mappedMemory, fillData, (size_t)_size);
    unmap();
    return true;
}

void vax::vk::Buffer::copyBufferCommand(
    vax::vk::CommandBuffer& commandBuffer, Buffer& dstBuffer, VkDeviceSize size
) const {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer.vkCommandBuffer, _vkBuffer, dstBuffer._vkBuffer, 1, &copyRegion);
}

bool Buffer::isEmpty() const { return _size == 0; }

bool Buffer::isAllocated() const { return _vkBuffer != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE; }

bool Buffer::_allocate(VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags) {
    if (isAllocated()) {
        return false;
    }

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = _size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo allocInfo{
        .flags = flags,
        .usage = memoryUsage,
    };

    VmaAllocationInfo resultAllocInfo{};
    if (vmaCreateBuffer(_device.get().allocator, &bufferInfo, &allocInfo, &_vkBuffer, &_allocation, &resultAllocInfo) !=
        VK_SUCCESS) {
        _logger.error("failed to create VMA buffer!");
        return false;
    }

    if (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) {
        _mappedMemory = resultAllocInfo.pMappedData;
        _isMapped = true;
    }

    if (!_name.empty()) {
        vmaSetAllocationName(_device.get().allocator, _allocation, _name.c_str());

        VkDebugUtilsObjectNameInfoEXT nameInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
            .objectType = VK_OBJECT_TYPE_BUFFER,
            .objectHandle = reinterpret_cast<uint64_t>(_vkBuffer),
            .pObjectName = _name.c_str(),
        };
        vax::vk::pfnSetDebugUtilsObjectNameEXT(_device.get().vkDevice, &nameInfo);
    }
    return true;
}

void Buffer::map() {
    if (isMapped())
        return;
    vmaMapMemory(_device.get().allocator, _allocation, &_mappedMemory);
    _isMapped = true;
}

void Buffer::unmap() {
    if (!isMapped())
        return;
    vmaUnmapMemory(_device.get().allocator, _allocation);
    _isMapped = false;
    _mappedMemory = nullptr;
}

std::optional<void*> Buffer::mappedMemory() const {
    if (!isMapped())
        return std::nullopt;
    return _mappedMemory;
}