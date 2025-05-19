#ifndef Buffer_hpp
#define Buffer_hpp

#include <iostream>

#include "luna.h"
#include "VKStack.hpp"

class Buffer {
public:
    VkBuffer vkBuffer;
    VkDeviceMemory vkBufferMemory;

    Buffer(
        VKStack *vkStack,
        VkDeviceSize size, 
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties);

    Buffer(const Buffer &rhs) = delete;
    Buffer(Buffer &&rhs) = delete;
    virtual ~Buffer();

    Buffer &operator=(Buffer &rhs) = delete;
    Buffer &operator=(Buffer &&rhs) = delete;

    void fill(const void *fillData);
    void copyBufferTo(VKStack *vkStack, Buffer& dstBuffer, VkDeviceSize size);

private:
    VkDevice &device;
    VkDeviceSize size;
};

#endif