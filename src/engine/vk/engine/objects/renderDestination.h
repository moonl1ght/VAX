#pragma once

#include "luna.h"
#include "device.h"
#include "texture.h"

namespace vax::vk {
    class RenderDestination final {
    public:
        std::unique_ptr<vax::textures::Texture> depthTexture;
        std::unique_ptr<vax::textures::Texture> drawImage;
        std::vector<VkFramebuffer> swapchainFramebuffers;

        explicit RenderDestination(
            const vax::vk::Device& device,
            std::unique_ptr<vax::textures::Texture> depthTexture,
            std::unique_ptr<vax::textures::Texture> drawImage,
            std::vector<VkFramebuffer> swapchainFramebuffers
        )
            : _device(device)
            , depthTexture(std::move(depthTexture))
            , drawImage(std::move(drawImage))
            , swapchainFramebuffers(std::move(swapchainFramebuffers)) {
        };

        RenderDestination(const RenderDestination& other) = delete;
        RenderDestination& operator=(const RenderDestination& other) = delete;
        RenderDestination(RenderDestination&& other) = delete;
        RenderDestination& operator=(RenderDestination&& other) = delete;

        ~RenderDestination() {
            destroy();
        }

    private:
        std::reference_wrapper<const vax::vk::Device> _device;

        void destroy();
    };
}