#include "commandBuffer.h"
#include "vkUtils.h"

using namespace vax::vk;

bool vax::vk::CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    return VK_CHECK(vkBeginCommandBuffer(vkCommandBuffer, &beginInfo));
}

bool vax::vk::CommandBuffer::end() {
    return VK_CHECK(vkEndCommandBuffer(vkCommandBuffer));
}