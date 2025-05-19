#ifndef Buffer_hpp
#define Buffer_hpp

#include "luna.h"
#include <iostream>

class Buffer {
public:
    Buffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory);
    virtual ~Buffer();

private:
};

#endif