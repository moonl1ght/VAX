#pragma once

#include "device.h"
#include "luna.h"
#include <variant>

namespace vax::vk {
class DescriptorSetLayout final {
  public:
    explicit DescriptorSetLayout(const vax::vk::Device& device, VkDescriptorSetLayout vkDescriptorSetLayout)
        : _device(device)
        , _vkDescriptorSetLayout(vkDescriptorSetLayout) {}

    ~DescriptorSetLayout() { vkDestroyDescriptorSetLayout(_device.get().vkDevice, _vkDescriptorSetLayout, nullptr); }

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
        : _device(other._device)
        , _vkDescriptorSetLayout(other._vkDescriptorSetLayout) {
        other._vkDescriptorSetLayout = VK_NULL_HANDLE;
    }

    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            _device = other._device;
            _vkDescriptorSetLayout = other._vkDescriptorSetLayout;
            other._vkDescriptorSetLayout = VK_NULL_HANDLE;
        }
        return *this;
    }

    VkDescriptorSetLayout getVkDescriptorSetLayout() const { return _vkDescriptorSetLayout; }

  private:
    std::reference_wrapper<const vax::vk::Device> _device;
    VkDescriptorSetLayout _vkDescriptorSetLayout = VK_NULL_HANDLE;
};
} // namespace vax::vk