#pragma once

#include "luna.h"
#include "renderPass.h"

namespace vax::vk {
    class Swapchain;
    class Device;

    class RenderPassBuilder {
    public:
        explicit RenderPassBuilder(
            const vax::vk::Device& device,
            const vax::vk::Swapchain& swapchain
        ) : _device(device), _swapchain(swapchain) {};

        std::optional<std::unique_ptr<vax::vk::RenderPass>> build() const noexcept;

    private:
        Logger _logger = Logger("RenderPassBuilder");
        std::reference_wrapper<const vax::vk::Device> _device;
        std::reference_wrapper<const vax::vk::Swapchain> _swapchain;
    };
}