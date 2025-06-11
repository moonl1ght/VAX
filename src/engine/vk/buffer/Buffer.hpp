#ifndef Buffer_hpp
#define Buffer_hpp

#include <iostream>

#include "VKStack.hpp"
#include "VKUtils.hpp"
#include "luna.h"

class Buffer final {
public:
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;
    VkDeviceSize size;

    Buffer(
        VKStack* vkStack,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) {
        std::swap(vkBuffer, other.vkBuffer);
        std::swap(vkBufferMemory, other.vkBufferMemory);
        std::swap(size, other.size);
        std::swap(_device, other._device);
    }
    ~Buffer();

    Buffer& operator=(Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) {
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

    void fill(const void* fillData);
    void copyBufferTo(VKStack* vkStack, Buffer& dstBuffer, VkDeviceSize size);

private:
    VkDevice _device;
};

#endif