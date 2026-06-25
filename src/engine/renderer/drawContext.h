#pragma once

#include "descriptorSetHandler.h"
#include "luna.h"

namespace vax::renderer {
struct DrawContext {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};
} // namespace vax::renderer
