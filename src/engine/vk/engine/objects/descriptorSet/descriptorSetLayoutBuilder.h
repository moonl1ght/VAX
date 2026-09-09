#pragma once

#include "descriptorSetLayout.h"
#include "device.h"
#include "luna.h"

namespace vax::vk {
class DescriptorSetLayoutBuilder final {
  public:
    explicit DescriptorSetLayoutBuilder(const vax::vk::Device& device, std::string_view name)
        : _device(device)
        , _name(name) {}

    ~DescriptorSetLayoutBuilder() {}

    DescriptorSetLayoutBuilder(const DescriptorSetLayoutBuilder&) = delete;
    DescriptorSetLayoutBuilder& operator=(const DescriptorSetLayoutBuilder&) = delete;
    DescriptorSetLayoutBuilder(DescriptorSetLayoutBuilder&& other) = delete;
    DescriptorSetLayoutBuilder& operator=(DescriptorSetLayoutBuilder&& other) = delete;

    void addBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t descriptorCount);

    void clear();

    std::optional<DescriptorSetLayout>
    build(VkDescriptorSetLayoutCreateFlags flags = 0);

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetLayoutBuilder");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;
    std::string_view _name;
};
} // namespace vax::vk