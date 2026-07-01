#include "descriptorSetHandler.h"
#include "buffer.h"
#include "texture.h"
#include "vkUtils.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetHandler::writeBuffer(
    const Buffer& buffer, uint32_t binding, uint32_t offset, VkDescriptorType descriptorType, uint32_t arrayElement
) {
    VkDescriptorBufferInfo& bufferInfo = _bufferInfos.emplace_back(
        VkDescriptorBufferInfo{.buffer = buffer.vkBuffer(), .offset = offset, .range = buffer.size()}
    );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = arrayElement,
        .descriptorCount = 1,
        .descriptorType = descriptorType,
        .pBufferInfo = &bufferInfo
    };

    _writes.push_back(write);
}

void DescriptorSetHandler::writeTexture(
    const Texture& texture, uint32_t binding, uint32_t arrayElement
) {
    auto imageInfoOpt = texture.descriptorImageInfoNoSampler();
    if (!imageInfoOpt) {
        _logger.error("Failed to write descriptor image info");
        return;
    }

    VkDescriptorImageInfo& imageInfo = _imageInfos.emplace_back(imageInfoOpt.value());

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = arrayElement,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &imageInfo
    };

    _writes.push_back(write);
}

void DescriptorSetHandler::writeTextures(const std::vector<const Texture*>& textures, uint32_t binding) {
    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.reserve(textures.size());
    for (const auto& texture : textures) {
        auto imageInfoOpt = texture->descriptorImageInfoNoSampler();
        if (!imageInfoOpt) {
            _logger.error("Failed to write descriptor image info");
            return;
        }
        imageInfos.push_back(*imageInfoOpt);
    }
    auto& imageInfosSaved = _imageInfosArray.emplace_back(imageInfos);

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = static_cast<uint32_t>(imageInfosSaved.size()),
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = imageInfosSaved.data()
    };

    _writes.push_back(write);
}

void DescriptorSetHandler::writeSampler(const Sampler& sampler, uint32_t binding, uint32_t arrayElement) {
    VkDescriptorImageInfo& samplerInfo = _imageInfos.emplace_back(
        VkDescriptorImageInfo{
            .sampler = sampler.vkSampler,
        }
    );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = arrayElement,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &samplerInfo
    };

    _writes.push_back(write);
}

VkDescriptorSet DescriptorSetHandler::update() {
    vkUpdateDescriptorSets(_device.get().vkDevice, static_cast<uint32_t>(_writes.size()), _writes.data(), 0, nullptr);
    return _descriptorSet;
}

void DescriptorSetHandler::clear() {
    _writes.clear();
    _bufferInfos.clear();
    _imageInfos.clear();
    _imageInfosArray.clear();
}