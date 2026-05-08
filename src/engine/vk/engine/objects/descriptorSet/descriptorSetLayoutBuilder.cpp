#include "descriptorSetLayoutBuilder.h"
#include "vkUtils.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetLayoutBuilder::addBinding(
    uint32_t binding,
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    uint32_t descriptorCount
) {
    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = descriptorCount,
        .stageFlags = stageFlags,
        .pImmutableSamplers = nullptr,
    };
    _bindings.push_back(layoutBinding);
}

void DescriptorSetLayoutBuilder::clear() {
    _bindings.clear();
}

std::optional<DescriptorSetLayout> DescriptorSetLayoutBuilder::build(
    DescriptorSetLayout::SetType setType,
    VkDescriptorSetLayoutCreateFlags flags
) {
    uint32_t bindingCount = static_cast<uint32_t>(_bindings.size());
    std::vector<VkDescriptorBindingFlags> bindingFlags(
        bindingCount,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
    );
    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = bindingCount,
        .pBindingFlags = bindingFlags.data(),
    };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &bindingFlagsInfo,
        .flags = flags | VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = bindingCount,
        .pBindings = _bindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayout;
    if (!VK_CHECK(vkCreateDescriptorSetLayout(_device.get().vkDevice, &layoutInfo, nullptr, &descriptorSetLayout))) {
        _logger.error("Failed to create descriptor set layout!");
        return std::nullopt;
    }

    return std::make_optional<DescriptorSetLayout>(_device, descriptorSetLayout, setType);
}