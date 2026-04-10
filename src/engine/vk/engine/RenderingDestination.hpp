#pragma once

#include "luna.h"
#include "swapchainManager.h"
#include "RenderPassManager.hpp"

class Texture;
namespace vax { class VkEngine; }

class RenderingDestination final {
public:
    Texture* depthTexture;
    std::unique_ptr<Texture> drawImage;
    std::vector<VkFramebuffer> swapchainFramebuffers;

    RenderingDestination(
        vax::VkEngine* vkEngine,
        vax::vk::SwapchainManager* swapchainManager,
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
    vax::VkEngine* _vkEngine;
    vax::vk::SwapchainManager* _swapchainManager;
    RenderPassManager* _renderPassManager;

    std::optional<Texture*> createDepthTexture(VkFormat format);

    bool createRenderDesctinationTexture(VkExtent2D windowExtent);
    bool createFramebuffers();
    bool createDepthResources();
};