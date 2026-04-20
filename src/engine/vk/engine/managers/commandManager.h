#pragma once

#include "luna.h"
#include "device.h"
#include "commandBuffer.h"

namespace vax::vk {
    class CommandManager final {
    public:

        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;

        explicit CommandManager(const vax::vk::Device& device) : _device(device) {};

        CommandManager(const CommandManager& other) = delete;
        CommandManager(CommandManager&& other) noexcept = delete;
        CommandManager& operator=(const CommandManager& other) = delete;
        CommandManager& operator=(CommandManager&& other) noexcept = delete;

        bool setup();

        void cleanup();

        vax::vk::CommandBuffer createSingleTimeCommandBuffer();

    private:
        Logger _logger = Logger("CommandManager");
        std::reference_wrapper<const vax::vk::Device> _device;

        bool createCommandPool();
        bool createCommandBuffer();
    };
}