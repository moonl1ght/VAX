#include "ssboManager.h"

using namespace vax::vk;

void SSBOManager::cleanup() {
    for (auto& buf : _buffers) {
        if (buf)
            buf->cleanup();
    }
}

bool SSBOManager::setup(uint32_t maxInstances) {
    _maxInstances = maxInstances;
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto allocation = vk::Buffer::allocate(
            _device.get(),
            "ssbo_instance_buffer",
            sizeof(InstanceData) * maxInstances,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        if (!allocation.has_value())
            return false;
        _buffers[i] = std::make_unique<vk::Buffer>(std::move(*allocation));
        _buffers[i]->map();
    }
    return true;
}

bool SSBOManager::updateInstance(uint32_t frameIndex, uint32_t index, const InstanceData& instance) {
    if (index >= _maxInstances)
        return false;
    auto& buf = _buffers[frameIndex];
    if (!buf)
        return false;
    auto mappedMemory = buf->mappedMemory();
    if (!mappedMemory.has_value())
        return false;
    InstanceData* instancePtr = static_cast<InstanceData*>(*mappedMemory);
    instancePtr[index] = instance;
    return true;
}
