#include "commandBuffer.h"
#include "vkUtils.h"

using namespace vax::vk;

bool vax::vk::CommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    auto result = VK_CHECK(vkBeginCommandBuffer(vkCommandBuffer, &beginInfo));
    if (result == VK_SUCCESS) {
        _isBegun = true;
    }
    return result;
}

bool vax::vk::CommandBuffer::end() {
    auto result = VK_CHECK(vkEndCommandBuffer(vkCommandBuffer));
    if (result == VK_SUCCESS) {
        _isBegun = false;
    }
    return result == VK_SUCCESS;
}