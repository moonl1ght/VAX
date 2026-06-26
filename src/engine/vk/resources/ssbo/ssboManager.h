#pragma once

#include "buffer.h"
#include "device.h"
#include "vkUtils.h"
#include <array>
#include <cstdint>
#include <limits>

namespace vax {
class SSBOManager final {
  public:
    using SSBOHandle = uint32_t;
    constexpr static SSBOHandle NullSSBOHandle = std::numeric_limits<SSBOHandle>::max();

    explicit SSBOManager(const vax::vk::Device& device)
        : _device(device) {};

    ~SSBOManager() { cleanup(); }

    SSBOManager(const SSBOManager& other) = delete;
    SSBOManager(SSBOManager&& other) noexcept = delete;
    SSBOManager& operator=(const SSBOManager& other) = delete;
    SSBOManager& operator=(SSBOManager&& other) noexcept = delete;

    bool setup(uint32_t maxInstances);

    void cleanup();

    bool updateInstance(uint32_t frameIndex, uint32_t index, const InstanceData& instance);

    const vax::vk::Buffer& instanceBuffer(uint32_t frameIndex) const { return *_buffers[frameIndex]; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("SSBOManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::array<std::unique_ptr<vax::vk::Buffer>, vax::MAX_FRAMES_IN_FLIGHT> _buffers;
    uint32_t _maxInstances = 0;
};
} // namespace vax
