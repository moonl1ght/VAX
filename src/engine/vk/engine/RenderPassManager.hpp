#pragma once

#include "SwapchainManager.hpp"
#include "device.h"

class RenderPassManager final {
public:
    RenderPassManager() = default;

    RenderPassManager(
        SwapchainManager* swapchainManager,
        vax::vk::Device* device
    )
        : _swapchainManager(swapchainManager)
        , _device(device) {
    };

    ~RenderPassManager() {};

    VkRenderPass getRenderPass() const {
        return _renderPass;
    }

    bool setup();
    void cleanup();

private:
    SwapchainManager* _swapchainManager;
    vax::vk::Device* _device;
    VkRenderPass _renderPass = VK_NULL_HANDLE;

    bool createRenderPass();
};