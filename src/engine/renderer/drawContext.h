#pragma once

#include "descriptorSetHandler.h"
#include "luna.h"

namespace vax::renderer {
struct DrawContext {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    uint32_t currentFrame = 0;
};
} // namespace vax::renderer
