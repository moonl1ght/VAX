#pragma once

#include "descriptorSetHandler.h"
#include "luna.h"

namespace vax::renderer {
struct DrawContext {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    vax::vk::DescriptorSetHandler* descriptorHandler = nullptr;
};
} // namespace vax::renderer
