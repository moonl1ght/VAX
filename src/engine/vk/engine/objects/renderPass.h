#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class RenderPass final {
    public:
        explicit RenderPass(
            const vax::vk::Device& device,
            VkRenderPass renderPass
        )
            : _device(device)
            , _renderPass(renderPass) {
        };

        RenderPass(const RenderPass& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;

        ~RenderPass() {
            vkDestroyRenderPass(_device.get().vkDevice, _renderPass, nullptr);
        };

        const VkRenderPass get_vk_render_pass() const {
            return _renderPass;
        }

    private:
        std::reference_wrapper<const vax::vk::Device> _device;
        VkRenderPass _renderPass = VK_NULL_HANDLE;
    };
}