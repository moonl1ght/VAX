#include "commandManager.h"
#include "vkUtils.h"
#include "vkEngine.h"

using namespace vax::vk;

bool vax::vk::CommandManager::setup() {
    if (!createCommandPool()) return false;
    if (!createCommandBuffer()) return false;
    return true;
}

void vax::vk::CommandManager::cleanup() {

    for (size_t i = 0; i < commandBuffers.size(); ++i) {
        vkFreeCommandBuffers(_device.get().vkDevice, commandPool, 1, &commandBuffers[i]);
    }

    vkDestroyCommandPool(_device.get().vkDevice, commandPool, nullptr);

    commandBuffers.clear();
    commandPool = VK_NULL_HANDLE;
}

bool vax::vk::CommandManager::createCommandPool() {
    _logger.info("Creating command pool...");
    utils::QueueFamilyIndices queueFamilyIndices = _device.get().getQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (!VK_CHECK(vkCreateCommandPool(_device.get().vkDevice, &poolInfo, nullptr, &commandPool))) {
        _logger.error("Failed to create command pool!");
        return false;
    }

    return true;
}

bool vax::vk::CommandManager::createCommandBuffer() {
    _logger.info("Creating command buffer...");
    commandBuffers.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (!VK_CHECK(vkAllocateCommandBuffers(_device.get().vkDevice, &allocInfo, commandBuffers.data()))) {
        _logger.error("Failed to allocate command buffers!");
        return false;
    }

    return true;
}

CommandBuffer vax::vk::CommandManager::createSingleTimeCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device.get().vkDevice, &allocInfo, &commandBuffer);

    return CommandBuffer(_device.get(), commandPool, commandBuffer);
}