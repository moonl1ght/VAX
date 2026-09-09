#pragma once

#include "device.h"
#include "luna.h"

namespace vax::vk {
struct DescriptorSetHandler {
  public:
    explicit DescriptorSetHandler(VkDescriptorSet descriptorSet)
        : _descriptorSet(descriptorSet) {}

    ~DescriptorSetHandler() {}

    DescriptorSetHandler(const DescriptorSetHandler&) = delete;
    DescriptorSetHandler& operator=(const DescriptorSetHandler&) = delete;
    DescriptorSetHandler(DescriptorSetHandler&& other) noexcept = delete;
    DescriptorSetHandler& operator=(DescriptorSetHandler&& other) noexcept = delete;

    void bind(
        VkCommandBuffer commandBuffer,
        VkPipelineLayout pipelineLayout,
        uint32_t setIndex,
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        uint32_t dynamicOffsetCount = 0,
        const uint32_t* pDynamicOffsets = nullptr
    ) {
        vkCmdBindDescriptorSets(
            commandBuffer, bindPoint, pipelineLayout, setIndex, 1, &_descriptorSet, dynamicOffsetCount, pDynamicOffsets
        );
    }

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetHandler");
    VkDescriptorSet _descriptorSet;
};
} // namespace vax::vk