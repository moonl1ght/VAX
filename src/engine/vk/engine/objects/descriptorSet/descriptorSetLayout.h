#pragma once

#include "device.h"
#include "luna.h"
#include <variant>

namespace vax::vk {
class DescriptorSetLayout final {
  public:
    enum class SetType {
        GLOBAL = 0,
        PER_FRAME = 1,
        FINAL_BLEND = 2,
    };

    explicit DescriptorSetLayout(
        const vax::vk::Device& device, VkDescriptorSetLayout vkDescriptorSetLayout, SetType setType
    )
        : _device(device)
        , _setType(setType)
        , _vkDescriptorSetLayout(vkDescriptorSetLayout) {}

    ~DescriptorSetLayout() { vkDestroyDescriptorSetLayout(_device.get().vkDevice, _vkDescriptorSetLayout, nullptr); }

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
        : _device(other._device)
        , _vkDescriptorSetLayout(other._vkDescriptorSetLayout)
        , _setType(other._setType) {
        other._vkDescriptorSetLayout = VK_NULL_HANDLE;
    }

    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            _device = other._device;
            _vkDescriptorSetLayout = other._vkDescriptorSetLayout;
            _setType = other._setType;
            other._vkDescriptorSetLayout = VK_NULL_HANDLE;
        }
        return *this;
    }

    VkDescriptorSetLayout getVkDescriptorSetLayout() const { return _vkDescriptorSetLayout; }

  private:
    std::reference_wrapper<const vax::vk::Device> _device;
    SetType _setType;
    VkDescriptorSetLayout _vkDescriptorSetLayout = VK_NULL_HANDLE;
};
} // namespace vax::vk