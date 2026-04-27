#include "descriptorSetManager.h"
#include "vkEngine.h"
#include "vkUtils.h"
#include "descriptorSetLayoutBuilder.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetManager::cleanup() {
    vkDestroyDescriptorPool(_device.get().vkDevice, _descriptorPool, nullptr);
    _defaultDescriptorSetLayouts.clear();
}

bool DescriptorSetManager::setup() {
    if (!createDefaultDescriptorSetLayouts()) {
        return false;
    }
    return createDescriptorSetPool();
}

bool DescriptorSetManager::createDescriptorSetPool() {
    uint32_t uniformBufferCount = 2;
    uint32_t imageSamplerCount = 1;
    uint32_t maxUniformBufferSets = static_cast<uint32_t>(_maxFramesInFlight) * uniformBufferCount;
    // uint32_t maxImageSamplerSets = static_cast<uint32_t>(_maxFramesInFlight) * imageSamplerCount;
    // uint32_t maxDrawBackgroundSets = static_cast<uint32_t>(_maxFramesInFlight) * 1;

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxUniformBufferSets },
        // { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxImageSamplerSets },
        // { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxDrawBackgroundSets }
    };

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(_maxFramesInFlight),
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    if (!VK_CHECK(vkCreateDescriptorPool(_device.get().vkDevice, &poolInfo, nullptr, &_descriptorPool))) {
        _logger.error("Failed to create descriptor pool!");
        return false;
    }

    return true;
}

std::optional<DescriptorSetWriter> DescriptorSetManager::getDefaultDescriptorSetWriter(
    uint32_t frameIndex, DescriptorSetLayout::DefaultType type
) {
    size_t typeIndex = static_cast<size_t>(type);
    if (_defaultDescriptorSets[typeIndex].size() == _maxFramesInFlight) {
        return std::make_optional<DescriptorSetWriter>(
            _device.get(),
            _defaultDescriptorSets[typeIndex][frameIndex]
        );
    }
    std::vector<VkDescriptorSetLayout> layouts(
        static_cast<size_t>(_maxFramesInFlight),
        _defaultDescriptorSetLayouts[typeIndex].getVkDescriptorSetLayout()
    );
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = _descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(_maxFramesInFlight),
        .pSetLayouts = layouts.data(),
    };
    _defaultDescriptorSets[typeIndex].resize(_maxFramesInFlight);
    auto result = vkAllocateDescriptorSets(
        _device.get().vkDevice, &allocInfo, _defaultDescriptorSets[typeIndex].data()
    );
    if (result != VK_SUCCESS) {
        _logger.error("Failed to allocate descriptor set!");
        return std::nullopt;
    }
    return std::make_optional<DescriptorSetWriter>(
        _device.get(),
        _defaultDescriptorSets[typeIndex][frameIndex]
    );
}

bool DescriptorSetManager::createDefaultDescriptorSetLayouts() {
    DescriptorSetLayoutBuilder builder(_device.get());
    builder.addBinding(
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    );
    builder.addBinding(
        1,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        VK_SHADER_STAGE_FRAGMENT_BIT
    );
    auto baseDescriptorSetLayout = builder.build(DescriptorSetLayout::DefaultType::BASE);
    builder.clear();
    if (!baseDescriptorSetLayout) {
        _logger.error("Failed to create base descriptor set layout!");
        return false;
    }
    _defaultDescriptorSetLayouts.push_back(std::move(baseDescriptorSetLayout.value()));
    return true;
}