#include "descriptorSetManager.h"
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
    uint32_t uniformBufferCount = 1;
    uint32_t materialBufferCount = 1;
    uint32_t samplerCount = vax::MAX_GLOBAL_SAMPLERS;
    uint32_t textureCount = vax::MAX_GLOBAL_TEXTURES;
    // auto samplersImageLimit = _device.get().getPhysicalDeviceProperties().limits.maxPerStageDescriptorSamplers;
    uint32_t maxUniformBuffers = static_cast<uint32_t>(_maxFramesInFlight) * uniformBufferCount;
    uint32_t maxMaterials = static_cast<uint32_t>(_maxFramesInFlight) * materialBufferCount;
    uint32_t maxTextures = static_cast<uint32_t>(_maxFramesInFlight) * textureCount;
    uint32_t maxSamplers = static_cast<uint32_t>(_maxFramesInFlight) * samplerCount;
    // maxImageSamplerSets = std::min(maxImageSamplerSets, samplersImageLimit);

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxUniformBuffers },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxMaterials },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxTextures },
        { VK_DESCRIPTOR_TYPE_SAMPLER, maxSamplers }
    };

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
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
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    builder.addBinding(
        1,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    builder.addBinding(
        2,
        VK_DESCRIPTOR_TYPE_SAMPLER,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        vax::MAX_GLOBAL_SAMPLERS
    );
    builder.addBinding(
        3,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        vax::MAX_GLOBAL_TEXTURES
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