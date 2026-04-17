#pragma once

#include "vkUtils.h"
#include "luna.h"
#include "device.h"

namespace vax::vk {
    class Engine;
}

namespace vax::vk
{
    class Buffer final
    {
    public:
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkBufferMemory = VK_NULL_HANDLE;
        VkDeviceSize size = 0;

        Buffer(const vax::vk::Device& device) : _device(device) {};

        explicit Buffer(
            const vax::vk::Device& device,
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties)
            : _device(device), size(size) {
            load(usage, properties);
            fill(data);
        }

        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) : _device(other._device) {
            std::swap(vkBuffer, other.vkBuffer);
            std::swap(vkBufferMemory, other.vkBufferMemory);
            std::swap(size, other.size);
        }
        ~Buffer() {
            cleanup();
        }

        Buffer& operator=(const Buffer& other) = delete;
        Buffer& operator=(Buffer&& other) noexcept
        {
            if (this != &other)
            {
                if (vkBuffer != VK_NULL_HANDLE)
                {
                    vkDestroyBuffer(_device.get().vkDevice, vkBuffer, nullptr);
                    vkBuffer = VK_NULL_HANDLE;
                }
                if (vkBufferMemory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(_device.get().vkDevice, vkBufferMemory, nullptr);
                    vkBufferMemory = VK_NULL_HANDLE;
                }
                vkBuffer = other.vkBuffer;
                vkBufferMemory = other.vkBufferMemory;
                size = other.size;
                _device = std::ref(other._device);
                other.vkBuffer = VK_NULL_HANDLE;
                other.vkBufferMemory = VK_NULL_HANDLE;
                other.size = 0;
            }
            return *this;
        }

        void load(
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        void reload(
            const void* data,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        void fill(const void* fillData);
        void copyBufferTo(vax::vk::Engine* vkEngine, Buffer& dstBuffer, VkDeviceSize size) const;
        bool isEmpty() const;
        bool isLoaded() const;

    private:
        Logger _logger = Logger("Buffer");
        std::reference_wrapper<const vax::vk::Device> _device;

        void load(
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        void cleanup();
    };
}