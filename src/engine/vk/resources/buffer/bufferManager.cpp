#include "bufferManager.h"
#include "deletionQueue.h"

using namespace vax;

bool BufferManager::fullCleanup() {
    for (auto& [id, buffer] : _pool) {
        buffer._destroy();
    }
    _pool.clear();
    return true;
}

std::optional<BufferManager::BufferResource> BufferManager::allocateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    auto buffer = vk::Buffer(_device.get());
    buffer._isDetached = false;
    buffer._size = size;
    buffer._id = _lastId++;
    if (!buffer._allocate(usage, properties)) return std::nullopt;
    auto [it, inserted] = _pool.try_emplace(buffer.id(), std::move(buffer));
    if (!inserted) {
        buffer._destroy();
        return std::nullopt;
    }
    return std::make_pair(it->first, &it->second);
}

std::optional<BufferManager::BufferResource> BufferManager::find(vax::BufferHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    return std::make_pair(handle, &it->second);
}

bool BufferManager::deleteBuffer(vax::BufferHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return false;
    it->second._destroy();
    _pool.erase(it);
    return true;
}

std::optional<vk::Buffer> BufferManager::detach(vax::BufferHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    it->second._detach();
    return std::move(it->second);
}
