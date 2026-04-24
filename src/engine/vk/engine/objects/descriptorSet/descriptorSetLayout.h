#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class DescriptorSetLayout final {
    public:
        enum class DefaultType {
            BASE = 0,
        };

        explicit DescriptorSetLayout(
            const vax::vk::Device& device,
            VkDescriptorSetLayout vkDescriptorSetLayout,
            DefaultType defaultType
        )
            : _device(device),
            _type(defaultType),
            _vkDescriptorSetLayout(vkDescriptorSetLayout) {
        }

        DescriptorSetLayout(
            const vax::vk::Device& device,
            VkDescriptorSetLayout vkDescriptorSetLayout,
            std::string name
        )
            : _device(device),
            _type(name),
            _vkDescriptorSetLayout(vkDescriptorSetLayout) {
        }

        ~DescriptorSetLayout() {
            vkDestroyDescriptorSetLayout(_device.get().vkDevice, _vkDescriptorSetLayout, nullptr);
        }

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
            : _device(other._device),
            _vkDescriptorSetLayout(other._vkDescriptorSetLayout),
            _type(other._type) {
            other._vkDescriptorSetLayout = VK_NULL_HANDLE;
        }

        DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept {
            if (this != &other) {
                _device = other._device;
                _vkDescriptorSetLayout = other._vkDescriptorSetLayout;
                _type = other._type;
                other._vkDescriptorSetLayout = VK_NULL_HANDLE;
            }
            return *this;
        }

        VkDescriptorSetLayout getVkDescriptorSetLayout() const { return _vkDescriptorSetLayout; }

    private:
        std::reference_wrapper<const vax::vk::Device> _device;
        VkDescriptorSetLayout _vkDescriptorSetLayout = VK_NULL_HANDLE;
        std::variant<DefaultType, std::string> _type;
    };
}