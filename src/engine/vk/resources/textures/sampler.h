#pragma once

#include "device.h"
#include "luna.h"

namespace vax::textures {
    class Sampler final {
    public:
        VkSampler vkSampler = VK_NULL_HANDLE;

        static std::optional<vax::textures::Sampler> createSampler(
            const vax::vk::Device& device,
            std::string name
        );

        explicit Sampler(
            VkSampler vkSampler,
            const vax::vk::Device& device
        ) : vkSampler(vkSampler), _device(device) {}

        Sampler(const Sampler& other) = delete;

        Sampler(Sampler&& other) noexcept
            : _device(other._device)
            , vkSampler(other.vkSampler)
        {
            other.vkSampler = VK_NULL_HANDLE;
        }

        Sampler& operator=(const Sampler& other) = delete;

        Sampler& operator=(Sampler&& other) noexcept {
            if (this != &other) {
                if (vkSampler != VK_NULL_HANDLE) {
                    vkDestroySampler(_device.get().vkDevice, vkSampler, nullptr);
                }
                _device = other._device;
                vkSampler = other.vkSampler;
                other.vkSampler = VK_NULL_HANDLE;
            }
            return *this;
        }

        ~Sampler() { vkDestroySampler(_device.get().vkDevice, vkSampler, nullptr); }

        void setName(const std::string& name) { _name = name; }

    private:
        vax::utils::Logger _logger = vax::utils::Logger("Sampler");
        std::string _name;
        std::reference_wrapper<const vax::vk::Device> _device;
    };
}