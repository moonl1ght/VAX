#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class SyncObjectsManager {
    public:
        SyncObjectsManager(const vax::vk::Device& device) : _device(device) {};

        bool setup();
        bool cleanup();

        const std::vector<VkSemaphore>& getImageAvailableSemaphores() const { return _imageAvailableSemaphores; }
        const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const { return _renderFinishedSemaphores; }
        const std::vector<VkFence>& getInFlightFences() const { return _inFlightFences; }

    private:
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;

        vax::utils::Logger _logger = vax::utils::Logger("SyncObjectsManager");
        std::reference_wrapper<const vax::vk::Device> _device;
    };
}