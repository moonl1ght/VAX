#pragma once

#include "buffer.h"
#include "commandBuffer.h"
#include "device.h"
#include "logger.h"
#include "luna.h"

namespace vax::engine {
class IndirectDrawController final {
  public:
    using IndirectDrawCommandBuffer = vk::Buffer<VkDrawIndexedIndirectCommand>;

    IndirectDrawController(const vk::Device& device)
        : _device(device) {};

    ~IndirectDrawController() = default;

    void setup(uint32_t maxCommands);

    void prepareForDraw(uint32_t frameIndex);

    void pushCommand(VkDrawIndexedIndirectCommand command);

    void submitCommands(uint32_t frameIndex);

    void draw(vk::CommandBuffer& commandBuffer, uint32_t frameIndex);

  private:
    vax::Logger _logger = vax::Logger("IndirectDrawController");

    std::reference_wrapper<const vk::Device> _device;

    std::vector<VkDrawIndexedIndirectCommand> _commands;

    std::vector<std::unique_ptr<IndirectDrawCommandBuffer>> _commandBuffers;

    uint32_t _maxCommands;

    std::vector<bool> _submitted;
};
} // namespace vax::engine