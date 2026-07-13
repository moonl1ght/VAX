#include "descriptorSetManager.h"
#include "descriptorSetLayoutBuilder.h"
#include "vkUtils.h"
#include <vulkan/vulkan_core.h>

using namespace vax::vk;
using namespace vax;

void DescriptorSetManager::cleanup() {
    vkDestroyDescriptorPool(_device.get().vkDevice, _descriptorPool, nullptr);
    _globalDescriptorSetLayout = std::nullopt;
    _perFrameDescriptorSetLayout = std::nullopt;
}

bool DescriptorSetManager::setup() {
    if (!_createDescriptorSetLayouts()) {
        return false;
    }
    return _createDescriptorSetPools();
}

bool DescriptorSetManager::_createDescriptorSetPools() {
    uint32_t uniformBufferCount = 1;
    uint32_t materialBufferCount = 1;
    uint32_t environmentMapCount = 1;
    uint32_t samplerCount = vax::vk::MAX_GLOBAL_SAMPLERS;
    uint32_t textureCount = vax::vk::MAX_GLOBAL_TEXTURES;
    // auto samplersImageLimit = _device.get().getPhysicalDeviceProperties().limits.maxPerStageDescriptorSamplers;
    uint32_t maxUniformBuffers = static_cast<uint32_t>(_maxFramesInFlight) * uniformBufferCount;
    uint32_t maxEnvironmentMaps = static_cast<uint32_t>(_maxFramesInFlight) * environmentMapCount;
    uint32_t maxMaterials = static_cast<uint32_t>(_maxFramesInFlight) * materialBufferCount;
    uint32_t maxTextures = static_cast<uint32_t>(_maxFramesInFlight) * textureCount;
    uint32_t maxSamplers = static_cast<uint32_t>(_maxFramesInFlight) * samplerCount;

    auto totalStorageBuffers = maxMaterials + maxEnvironmentMaps;
    // maxImageSamplerSets = std::min(maxImageSamplerSets, samplersImageLimit);

    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxUniformBuffers},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, totalStorageBuffers},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxTextures},
        {VK_DESCRIPTOR_TYPE_SAMPLER, maxSamplers},
    };

    uint32_t totalSetsPerFrame =
        uniformBufferCount + materialBufferCount + environmentMapCount + textureCount + samplerCount;
    uint32_t maxAllocatedSets = static_cast<uint32_t>(_maxFramesInFlight) * totalSetsPerFrame;
    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = maxAllocatedSets,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    if (!VK_CHECK(vkCreateDescriptorPool(_device.get().vkDevice, &poolInfo, nullptr, &_descriptorPool))) {
        _logger.error("Failed to create descriptor pool!");
        return false;
    }

    uint32_t numberOfProcessingImages = 6;

    uint32_t maxProcessingImages = static_cast<uint32_t>(_maxFramesInFlight) * numberOfProcessingImages;
    std::vector<VkDescriptorPoolSize> processingDescriptorPoolSizes = {
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxProcessingImages},
    };

    VkDescriptorPoolCreateInfo processingDescriptorPoolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = maxProcessingImages,
        .poolSizeCount = static_cast<uint32_t>(processingDescriptorPoolSizes.size()),
        .pPoolSizes = processingDescriptorPoolSizes.data(),
    };

    if (!VK_CHECK(vkCreateDescriptorPool(
            _device.get().vkDevice, &processingDescriptorPoolInfo, nullptr, &_processingDescriptorPool
        ))) {
        _logger.error("Failed to create processing descriptor pool!");
        return false;
    }

    uint32_t numberOfCombinedImages = 1;
    uint32_t numberOfStorageImages = 3;
    uint32_t maxCombinedImageSamplers = static_cast<uint32_t>(_maxFramesInFlight) * numberOfCombinedImages;
    uint32_t maxStorageImages = static_cast<uint32_t>(_maxFramesInFlight) * numberOfStorageImages;
    std::vector<VkDescriptorPoolSize> finalBlendDescriptorPoolSizes = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxCombinedImageSamplers},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxStorageImages},
    };

    VkDescriptorPoolCreateInfo finalBlendDescriptorPoolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = maxCombinedImageSamplers + maxStorageImages,
        .poolSizeCount = static_cast<uint32_t>(finalBlendDescriptorPoolSizes.size()),
        .pPoolSizes = finalBlendDescriptorPoolSizes.data(),
    };

    if (!VK_CHECK(vkCreateDescriptorPool(
            _device.get().vkDevice, &finalBlendDescriptorPoolInfo, nullptr, &_finalBlendDescriptorPool
        ))) {
        _logger.error("Failed to create descriptor pool!");
        return false;
    }

    return true;
}

const DescriptorSetLayout* DescriptorSetManager::getDescriptorSetLayout(DescriptorSetLayout::SetType setType) const {
    if (setType == DescriptorSetLayout::SetType::GLOBAL) {
        return &_globalDescriptorSetLayout.value();
    } else if (setType == DescriptorSetLayout::SetType::PER_FRAME) {
        return &_perFrameDescriptorSetLayout.value();
    }
    return nullptr;
}

std::optional<DescriptorSetHandler> createOrGetDescriptorSet(
    const Device& device,
    uint32_t idOffset,
    std::vector<VkDescriptorSet>& descriptorSets,
    const DescriptorSetLayout& descriptorSetLayout,
    const VkDescriptorPool descriptorPool,
    const uint32_t maxFramesInFlight,
    const uint32_t frameIndex
) {
    uint32_t descriptorSetId = idOffset * maxFramesInFlight + frameIndex;
    if (descriptorSets.size() == maxFramesInFlight) {
        return std::make_optional<DescriptorSetHandler>(device, descriptorSets[frameIndex], descriptorSetId);
    }
    std::vector<VkDescriptorSetLayout> layouts(
        static_cast<size_t>(maxFramesInFlight), descriptorSetLayout.getVkDescriptorSetLayout()
    );
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight),
        .pSetLayouts = layouts.data(),
    };
    descriptorSets.resize(maxFramesInFlight, VK_NULL_HANDLE);
    auto result = vkAllocateDescriptorSets(device.vkDevice, &allocInfo, descriptorSets.data());
    if (result != VK_SUCCESS) {
        return std::nullopt;
    }
    return std::make_optional<DescriptorSetHandler>(device, descriptorSets[frameIndex], descriptorSetId);
}

std::optional<DescriptorSetHandler>
DescriptorSetManager::getDescriptorSetHandler(uint32_t frameIndex, DescriptorSetLayout::SetType setType) {
    switch (setType) {
    case DescriptorSetLayout::SetType::GLOBAL:
        return createOrGetDescriptorSet(
            _device.get(),
            0,
            _globalDescriptorSets,
            _globalDescriptorSetLayout.value(),
            _descriptorPool,
            _maxFramesInFlight,
            frameIndex
        );
    case DescriptorSetLayout::SetType::PER_FRAME:
        return createOrGetDescriptorSet(
            _device.get(),
            1,
            _perFrameDescriptorSets,
            _perFrameDescriptorSetLayout.value(),
            _descriptorPool,
            _maxFramesInFlight,
            frameIndex
        );
    default:
        _logger.error("Invalid descriptor set type!");
        return std::nullopt;
    }
}

std::optional<DescriptorSetHandler> DescriptorSetManager::getDescriptorSetHandler(
    uint32_t frameIndex, PoolType poolType, std::string name, std::string layoutName
) {
    auto& sets = _descriptorSets[name];
    auto descriptorSetLayout = _descriptorSetLayouts.find(layoutName);
    if (descriptorSetLayout == _descriptorSetLayouts.end()) {
        _logger.error("Descriptor set layout not found!");
        return std::nullopt;
    }
    switch (poolType) {
    case PoolType::PROCESSING:
        return createOrGetDescriptorSet(
            _device.get(),
            0,
            sets,
            descriptorSetLayout->second,
            _processingDescriptorPool,
            _maxFramesInFlight,
            frameIndex
        );
    case PoolType::FINAL_BLEND:
        return createOrGetDescriptorSet(
            _device.get(),
            0,
            sets,
            descriptorSetLayout->second,
            _finalBlendDescriptorPool,
            _maxFramesInFlight,
            frameIndex
        );
    default:
        _logger.error("Invalid pool type!");
        return std::nullopt;
    }
}

bool DescriptorSetManager::_createDescriptorSetLayouts() {
    DescriptorSetLayoutBuilder globalBuilder(_device.get(), "global_descriptor_set_layout");
    globalBuilder.addBinding(
        GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    globalBuilder.addBinding(
        GlobalBindingIndices::GLOBAL_ENVIRONMENT_MAP_BUFFER_INDEX,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    globalBuilder.addBinding(
        GlobalBindingIndices::GLOBAL_SAMPLER_INDEX,
        VK_DESCRIPTOR_TYPE_SAMPLER,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        vax::vk::MAX_GLOBAL_SAMPLERS
    );
    globalBuilder.addBinding(
        GlobalBindingIndices::GLOBAL_TEXTURE_INDEX,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        vax::vk::MAX_GLOBAL_TEXTURES
    );
    DescriptorSetLayoutBuilder perFrameBuilder(_device.get(), "per_frame_descriptor_set_layout");
    perFrameBuilder.addBinding(
        FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    perFrameBuilder.addBinding(
        FrameBindingIndices::FRAME_INSTANCE_BUFFER_INDEX,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        1
    );
    auto globalDescriptorSetLayout = globalBuilder.build(DescriptorSetLayout::SetType::GLOBAL);
    auto perFrameDescriptorSetLayout = perFrameBuilder.build(DescriptorSetLayout::SetType::PER_FRAME);
    globalBuilder.clear();
    perFrameBuilder.clear();
    if (!globalDescriptorSetLayout || !perFrameDescriptorSetLayout) {
        _logger.error("Failed to create base descriptor set layout!");
        return false;
    }
    _globalDescriptorSetLayout = std::move(globalDescriptorSetLayout.value());
    _perFrameDescriptorSetLayout = std::move(perFrameDescriptorSetLayout.value());

    DescriptorSetLayoutBuilder finalBlendSampledBuilder(_device.get(), "final_blend_sampled_descriptor_set_layout");
    finalBlendSampledBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
    finalBlendSampledBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
    auto finalBlendSampledDescriptorSetLayout = finalBlendSampledBuilder.build(DescriptorSetLayout::SetType::OTHER);
    if (!finalBlendSampledDescriptorSetLayout) {
        _logger.error("Failed to create final blend descriptor set layout!");
        return false;
    }
    _descriptorSetLayouts.insert({"final_blend_sampled", std::move(finalBlendSampledDescriptorSetLayout.value())});

    DescriptorSetLayoutBuilder finalBlendStorageBuilder(_device.get(), "final_blend_descriptor_set_layout");
    finalBlendStorageBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
    auto finalBlendStorageDescriptorSetLayout = finalBlendStorageBuilder.build(DescriptorSetLayout::SetType::OTHER);
    if (!finalBlendStorageDescriptorSetLayout) {
        _logger.error("Failed to create final blend descriptor set layout!");
        return false;
    }
    _descriptorSetLayouts.insert({"final_blend", std::move(finalBlendStorageDescriptorSetLayout.value())});
    return true;
}

void DescriptorSetManager::addDescriptorSetLayout(std::string name, DescriptorSetLayout&& layout) {
    _descriptorSetLayouts.insert({name, std::move(layout)});
}

const DescriptorSetLayout* DescriptorSetManager::getDescriptorSetLayout(std::string name) const {
    auto it = _descriptorSetLayouts.find(name);
    if (it == _descriptorSetLayouts.end()) {
        return nullptr;
    }
    return &it->second;
}