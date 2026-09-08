#include "indirectDrawController.h"

using namespace vax::engine;
using namespace vax::vk;
using namespace vax;

void IndirectDrawController::setup(uint32_t maxCommands) {
    _maxCommands = maxCommands;
    _commands.reserve(maxCommands);
    for (uint32_t i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer = IndirectDrawCommandBuffer::allocate(
            _device.get(),
            "indirect_draw_commands",
            _maxCommands * sizeof(VkDrawIndexedIndirectCommand),
            VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU
        );
        if (!buffer) {
            _logger.error("Failed to allocate indirect draw command buffer");
            return;
        }
        _commandBuffers.push_back(std::make_unique<IndirectDrawCommandBuffer>(std::move(*buffer)));
        _submitted.push_back(false);
    }
}

void IndirectDrawController::draw(CommandBuffer& commandBuffer, uint32_t frameIndex) {
    if (!_submitted[frameIndex]) {
        _logger.error("Commands not submitted");
        return;
    }
    vkCmdDrawIndexedIndirect(
        commandBuffer.vkCommandBuffer,
        _commandBuffers[frameIndex]->vkBuffer(),
        0,
        static_cast<uint32_t>(_commands.size()),
        sizeof(VkDrawIndexedIndirectCommand)
    );
}

void IndirectDrawController::pushCommand(VkDrawIndexedIndirectCommand command) {
    if (_commands.size() >= _maxCommands) {
        _logger.error("Max commands reached");
        return;
    }
    _commands.push_back(command);
}

void IndirectDrawController::submitCommands(uint32_t frameIndex) {
    if (_commands.empty()) {
        return;
    }
    _commandBuffers[frameIndex]->fill(_commands.data(), _commands.size() * sizeof(VkDrawIndexedIndirectCommand));
    _submitted[frameIndex] = true;
}

void IndirectDrawController::prepareForDraw(uint32_t frameIndex) {
    _commands.clear();
    _submitted[frameIndex] = false;
}