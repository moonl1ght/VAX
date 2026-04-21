#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class QueueManager final {
    public:
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        QueueManager() { }

        QueueManager(const QueueManager& other) = delete;
        QueueManager(QueueManager&& other) = delete;
        QueueManager& operator=(const QueueManager& other) = delete;
        QueueManager& operator=(QueueManager&& other) = delete;

        void setup(const Device& device);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("QueueManager");
    };
}