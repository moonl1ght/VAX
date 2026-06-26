#pragma once

#include "device.h"
#include "buffer.h"

namespace vax {
class SSBOManager final {
  public:
    explicit SSBOManager(const vax::vk::Device& device)
        : _device(device) {};

    ~SSBOManager() { cleanup(); }

    SSBOManager(const SSBOManager& other) = delete;
    SSBOManager(SSBOManager&& other) noexcept = delete;
    SSBOManager& operator=(const SSBOManager& other) = delete;
    SSBOManager& operator=(SSBOManager&& other) noexcept = delete;

    bool setup(uint32_t maxInstances);

    void cleanup();

    bool updateInstance(uint32_t index, const InstanceData& instance);

    const vax::vk::Buffer& instanceBuffer() const { return *_buffer; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("SSBOManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::unique_ptr<vax::vk::Buffer> _buffer = nullptr;
    uint32_t _maxInstances = 0;
};
} // namespace vax