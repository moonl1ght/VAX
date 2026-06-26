#include "ssboManager.h"


using namespace vax;

void SSBOManager::cleanup() { _buffer->cleanup(); }

bool SSBOManager::setup(uint32_t maxInstances) {
    auto allocation = vk::Buffer::allocate(
        _device.get(),
        "ssbo_instance_buffer",
        sizeof(InstanceData) * maxInstances,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    if (!allocation.has_value())
        return false;
    _buffer = std::make_unique<vk::Buffer>(std::move(*allocation));
    _buffer->map();
    _maxInstances = maxInstances;
    return true;
}

bool SSBOManager::updateInstance(uint32_t index, const InstanceData& instance) {
    if (index >= _maxInstances)
        return false;
    if (!_buffer)
        return false;
    auto mappedMemory = _buffer->mappedMemory();
    if (!mappedMemory.has_value())
        return false;
    InstanceData* instancePtr = static_cast<InstanceData*>(*mappedMemory);
    instancePtr[index] = instance;
    return true;
}