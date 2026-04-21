#include "syncObjectsManager.h"
#include "vkUtils.h"
#include "vkEngine.h"

using namespace vax::vk;

bool vax::vk::SyncObjectsManager::setup() {
    _logger.info("Creating synchronization objects...");
    _imageAvailableSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(Engine::MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < Engine::MAX_FRAMES_IN_FLIGHT; i++) {
        if (!VK_CHECK(
            vkCreateSemaphore(_device.get().vkDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i])
        )) {
            return false;
        }
        if (!VK_CHECK(
            vkCreateSemaphore(_device.get().vkDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i])
        )) {
            return false;
        }
        if (!VK_CHECK(vkCreateFence(_device.get().vkDevice, &fenceInfo, nullptr, &_inFlightFences[i]))) {
            return false;
        }
    }

    return true;
}

bool vax::vk::SyncObjectsManager::cleanup() {
    for (size_t i = 0; i < _imageAvailableSemaphores.size(); ++i) {
        vkDestroySemaphore(_device.get().vkDevice, _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_device.get().vkDevice, _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_device.get().vkDevice, _inFlightFences[i], nullptr);
    }
    _imageAvailableSemaphores.clear();
    _renderFinishedSemaphores.clear();
    _inFlightFences.clear();
    return true;
}