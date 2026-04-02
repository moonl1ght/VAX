#ifndef Buffer_hpp
#define Buffer_hpp

#include <iostream>

#include "vkEngine.h"
#include "VKUtils.hpp"
#include "luna.h"

class Buffer final {
public:
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;

    Buffer() {}

    Buffer(
        vax::VkEngine* vkEngine,
        const void* data,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) noexcept {
        std::swap(vkBuffer, other.vkBuffer);
        std::swap(vkBufferMemory, other.vkBufferMemory);
        std::swap(size, other.size);
        std::swap(_device, other._device);
    }
    ~Buffer();

    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (vkBuffer != VK_NULL_HANDLE) {
                vkDestroyBuffer(_device, vkBuffer, nullptr);
                vkBuffer = VK_NULL_HANDLE;
            }
            if (vkBufferMemory != VK_NULL_HANDLE) {
                vkFreeMemory(_device, vkBufferMemory, nullptr);
                vkBufferMemory = VK_NULL_HANDLE;
            }
            vkBuffer = other.vkBuffer;
            vkBufferMemory = other.vkBufferMemory;
            size = other.size;
            _device = other._device;
            other.vkBuffer = VK_NULL_HANDLE;
            other.vkBufferMemory = VK_NULL_HANDLE;
            other.size = 0;
            other._device = VK_NULL_HANDLE;
        }
        return *this;
    }

    void load(
        vax::VkEngine* vkEngine,
        const void* data,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
    void reload(
        vax::VkEngine* vkEngine,
        const void* data,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
    void fill(const void* fillData);
    void copyBufferTo(vax::VkEngine* vkEngine, Buffer& dstBuffer, VkDeviceSize size) const;
    bool isEmpty() const;
    bool isLoaded() const;

private:
    VkDevice _device;

    void load(
        vax::VkEngine* vkEngine,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    void cleanup();
};

#endif