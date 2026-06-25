#pragma once

#include "device.h"

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

    bool setup();

    void cleanup();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("SSBOManager");
    std::reference_wrapper<const vax::vk::Device> _device;
};
} // namespace vax