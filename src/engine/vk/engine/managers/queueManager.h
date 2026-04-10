#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class QueueManager final {
    public:
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        QueueManager() {
            _logger = Logger("QueueManager");
        }

        void setup(const Device& device);

    private:
        Logger _logger;
    };
}