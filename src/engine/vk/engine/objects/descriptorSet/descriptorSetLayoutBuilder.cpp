#include "descriptorSetLayoutBuilder.h"
#include "vkUtils.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetLayoutBuilder::addBinding(
    uint32_t binding,
    VkDescriptorType type,
    VkShaderStageFlags stageFlags
) {
    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = 1,
        .stageFlags = stageFlags,
        .pImmutableSamplers = nullptr,
    };
    _bindings.push_back(layoutBinding);
}

void DescriptorSetLayoutBuilder::clear() {
    _bindings.clear();
}

std::optional<DescriptorSetLayout> DescriptorSetLayoutBuilder::build(
    DescriptorSetLayout::DefaultType defaultType,
    void* pNext,
    VkDescriptorSetLayoutCreateFlags flags
) {
    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = pNext,
        .flags = flags,
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings = _bindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayout;
    if (!VK_CHECK(vkCreateDescriptorSetLayout(_device.get().vkDevice, &layoutInfo, nullptr, &descriptorSetLayout))) {
        _logger.error("Failed to create descriptor set layout!");
        return std::nullopt;
    }

    return std::make_optional<DescriptorSetLayout>(_device, descriptorSetLayout, defaultType);
}