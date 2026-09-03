#pragma once

#include "buffer.h"
#include "device.h"
#include "luna.h"
#include "resourceHandle.h"

namespace vax::vk {
class BufferManager final {
  public:
    using BufferResource = std::pair<BufferHandle, Buffer*>;

    explicit BufferManager(const Device& device)
        : _device(device) {};

    ~BufferManager() { fullCleanup(); }

    BufferManager(const BufferManager& other) = delete;
    BufferManager(BufferManager&& other) noexcept = delete;
    BufferManager& operator=(const BufferManager& other) = delete;
    BufferManager& operator=(BufferManager&& other) noexcept = delete;

    void fullCleanup();

    std::optional<BufferResource> allocateBuffer(
        std::string name,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags flags = 0
    );

    std::optional<BufferResource> find(BufferHandle handle);

    bool deleteBuffer(BufferHandle handle);

    std::optional<Buffer> detach(BufferHandle handle);

  private:
    vax::Logger _logger = vax::Logger("BufferManager");

    std::reference_wrapper<const Device> _device;
    // TODO: change to vector + use generation for stability
    // maybe vector of vectors of buffers?
    std::unordered_map<BufferId, Buffer> _pool;
    BufferId _lastId = 0;
};
} // namespace vax::vk