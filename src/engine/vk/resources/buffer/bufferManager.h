#pragma once

#include "luna.h"
#include "device.h"
#include "buffer.h"
#include "bufferHandle.h"

namespace vax {
    class BufferManager final {
    public:
        using BufferResource = std::pair<vax::BufferHandle, vax::vk::Buffer*>;

        explicit BufferManager(const vax::vk::Device& device) : _device(device) {};

        ~BufferManager() {
            fullCleanup();
        }

        BufferManager(const BufferManager& other) = delete;
        BufferManager(BufferManager&& other) noexcept = delete;
        BufferManager& operator=(const BufferManager& other) = delete;
        BufferManager& operator=(BufferManager&& other) noexcept = delete;

        bool fullCleanup();

        std::optional<BufferResource> allocateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        std::optional<BufferResource> find(vax::BufferHandle handle);

        bool deleteBuffer(vax::BufferHandle handle);

        std::optional<vk::Buffer> detach(vax::BufferHandle handle);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("BufferManager");

        std::reference_wrapper<const vax::vk::Device> _device;
        // TODO: change to vector + use generation for stability
        // maybe vector of vectors of buffers?
        std::unordered_map<BufferId, vax::vk::Buffer> _pool;
        BufferId _lastId = 1;
    };
}