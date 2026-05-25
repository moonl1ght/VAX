#pragma once

#include "bufferData.h"
#include "device.h"
#include "luna.h"
#include "resourceUtils.h"
#include "vkUtils.h"

namespace vax::vk {
class CommandBuffer;
}

namespace vax {
class BufferManager;
}

namespace vax::vk {
class Buffer final {
  public:
    friend class vax::BufferManager;

    static std::optional<Buffer> allocateAndFillData(
        const vax::vk::Device& device,
        std::string name,
        const void* data,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    static std::optional<Buffer> allocate(
        const vax::vk::Device& device,
        std::string name,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    ~Buffer() { cleanup(); }

    Buffer(const vax::vk::Device& device)
        : _device(device) {};

    Buffer(const Buffer& other) = delete;

    Buffer& operator=(const Buffer& other) = delete;

    Buffer(Buffer&& other)
        : _device(other._device)
        , _vkBuffer(other._vkBuffer)
        , _vkBufferMemory(other._vkBufferMemory)
        , _size(other._size)
        , _isDetached(other._isDetached)
        , _id(other._id) {
        other._vkBuffer = VK_NULL_HANDLE;
        other._vkBufferMemory = VK_NULL_HANDLE;
        other._size = 0;
        other._isDetached = true;
        other._id = vax::NullId;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            _device = other._device;
            _vkBuffer = other._vkBuffer;
            _vkBufferMemory = other._vkBufferMemory;
            _size = other._size;
            _isDetached = other._isDetached;
            _id = other._id;
            other._vkBuffer = VK_NULL_HANDLE;
            other._vkBufferMemory = VK_NULL_HANDLE;
            other._size = 0;
            other._isDetached = true;
            other._id = vax::NullId;
        }
        return *this;
    }

    bool load(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    bool reload(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    bool fill(const void* fillData);

    void map();

    void unmap();

    std::optional<void*> mappedMemory() const;

    void copyBufferCommand(vax::vk::CommandBuffer& commandBuffer, Buffer& dstBuffer, VkDeviceSize size) const;

    bool isEmpty() const;

    bool isMapped() const { return _isMapped; }

    bool isAllocated() const;

    void cleanup();

    VkBuffer vkBuffer() const { return _vkBuffer; }

    VkDeviceMemory vkBufferMemory() const { return _vkBufferMemory; }

    VkDeviceSize size() const { return _size; }

    BufferId id() const { return _id; }

    bool isDetached() const { return _isDetached; }

    const std::string& name() const { return _name; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("Buffer");
    std::reference_wrapper<const vax::vk::Device> _device;

    std::string _name;
    BufferId _id = vax::NullBufferId;
    VkBuffer _vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory _vkBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;
    bool _isMapped = false;
    void* _mappedMemory = nullptr;
    bool _isDetached = true;

    bool _allocate(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    void _destroy();

    void _detach();
};
} // namespace vax::vk