#pragma once

#include "luna.h"
#include "indirectDrawController.h"

namespace vax::engine {
struct DrawContext {
    vax::vk::CommandBuffer& commandBuffer;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    uint32_t currentFrame = 0;
    IndirectDrawController* indirectDrawController = nullptr;
};
} // namespace vax::engine
