#pragma once

#include "device.h"
#include "luna.h"
#include "texture.h"

namespace vax::vk {
class RenderDestination final {
  public:
    std::vector<VkFramebuffer> framebuffers;

    explicit RenderDestination(
        const Device& device,
        std::unique_ptr<Texture> depthTexture,
        std::vector<Texture> textures,
        std::vector<VkFramebuffer> framebuffers
    )
        : _device(device)
        , _depthTexture(std::move(depthTexture))
        , _textures(std::move(textures))
        , framebuffers(std::move(framebuffers)) {}

    RenderDestination(const RenderDestination& other) = delete;
    RenderDestination& operator=(const RenderDestination& other) = delete;

    RenderDestination(RenderDestination&& other) = default;
    RenderDestination& operator=(RenderDestination&& other) = default;

    ~RenderDestination() { destroy(); }

    const std::vector<Texture>& textures() const { return _textures; }

  private:
    std::reference_wrapper<const Device> _device;
    std::vector<Texture> _textures;
    std::unique_ptr<Texture> _depthTexture;

    void destroy();
};
} // namespace vax::vk