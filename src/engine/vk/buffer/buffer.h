#pragma once

#include "vkUtils.h"
#include "luna.h"
#include "device.h"
#include "bufferData.h"

namespace vax::vk {
    class QueueManager;
    class CommandManager;
}

namespace vax::vk {
    class Buffer final {
    public:

        static std::optional<Buffer> allocateAndFillData(
            const vax::vk::Device& device,
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        static std::optional<Buffer> allocate(
            const vax::vk::Device& device,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        ~Buffer() {
            cleanup();
        }

        Buffer(const vax::vk::Device& device) : _device(device) {};

        Buffer(const Buffer& other) = delete;

        Buffer& operator=(const Buffer& other) = delete;

        Buffer(Buffer&& other)
            : _device(other._device)
            , _vkBuffer(other._vkBuffer)
            , _vkBufferMemory(other._vkBufferMemory)
            , _size(other._size) {
            other._vkBuffer = VK_NULL_HANDLE;
            other._vkBufferMemory = VK_NULL_HANDLE;
            other._size = 0;
        }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                cleanup();
                _device = other._device;
                _vkBuffer = other._vkBuffer;
                _vkBufferMemory = other._vkBufferMemory;
                _size = other._size;
                other._vkBuffer = VK_NULL_HANDLE;
                other._vkBufferMemory = VK_NULL_HANDLE;
                other._size = 0;
            }
            return *this;
        }

        void bind(void* data);

        bool load(
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        bool reload(
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        bool fill(const void* fillData);

        bool copyBufferTo(
            const QueueManager& queueManager,
            CommandManager& commandManager,
            Buffer& dstBuffer,
            VkDeviceSize size
        ) const;

        bool isEmpty() const;

        bool isGpuAllocated() const;

        bool cleanup();

        VkBuffer getVkBuffer() const { return _vkBuffer; }

        VkDeviceMemory getVkBufferMemory() const { return _vkBufferMemory; }

        VkDeviceSize getSize() const { return _size; }

    private:
        Logger _logger = Logger("Buffer");
        std::reference_wrapper<const vax::vk::Device> _device;

        VkBuffer _vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory _vkBufferMemory = VK_NULL_HANDLE;
        VkDeviceSize _size = 0;

        bool _allocateOnGpu(
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );
    };
}