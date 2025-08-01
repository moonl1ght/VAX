#pragma once

#include "luna.h"
#include "SwapchainManager.hpp"
#include "RenderPassManager.hpp"

class Texture;
class VKEngine;

class RenderingDestination final {
public:
    Texture* depthTexture;
    std::vector<VkFramebuffer> swapchainFramebuffers;

    RenderingDestination(
        VKEngine* vkEngine,
        SwapchainManager* swapchainManager,
        RenderPassManager* renderPassManager
    )
        : _vkEngine(vkEngine)
        , _swapchainManager(swapchainManager)
        , _renderPassManager(renderPassManager) {
    };

    ~RenderingDestination() = default;

    bool setup();
    void cleanup();
    void recreate();

private:
    VKEngine* _vkEngine;
    SwapchainManager* _swapchainManager;
    RenderPassManager* _renderPassManager;

    std::optional<Texture*> createDepthTexture(VkFormat format);
    std::optional<Texture*> createRenderDesctinationTexture(VkExtent2D windowExtent);

    bool createFramebuffers();
    bool createDepthResources();
};