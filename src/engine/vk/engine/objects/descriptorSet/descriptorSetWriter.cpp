#include "descriptorSetWriter.h"
#include "buffer.h"
#include "texture.h"
#include "vkUtils.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetWriter::writeBuffer(
    const Buffer& buffer,
    uint32_t binding,
    uint32_t offset,
    VkDescriptorType descriptorType,
    uint32_t descriptorCount
) {
    VkDescriptorBufferInfo& bufferInfo = _bufferInfos.emplace_back(VkDescriptorBufferInfo{
        .buffer = buffer.vkBuffer(),
        .offset = offset,
        .range = buffer.size()
    });

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = descriptorCount,
        .descriptorType = descriptorType,
        .pBufferInfo = &bufferInfo
    };

    _writes.push_back(write);
}

void DescriptorSetWriter::writeTexture(
    const vax::textures::Texture& texture,
    uint32_t binding,
    VkDescriptorType descriptorType,
    uint32_t descriptorCount
) {
    auto imageInfoOpt = texture.descriptorImageInfo();
    if (!imageInfoOpt) {
        _logger.error("Failed to write descriptor image info");
        return;
    }

    VkDescriptorImageInfo& imageInfo = _imageInfos.emplace_back(imageInfoOpt.value());

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = descriptorCount,
        .descriptorType = descriptorType,
        .pImageInfo = &imageInfo
    };

    _writes.push_back(write);
}

VkDescriptorSet DescriptorSetWriter::update() {
    vkUpdateDescriptorSets(
        _device.get().vkDevice,
        static_cast<uint32_t>(_writes.size()),
        _writes.data(),
        0,
        nullptr
    );
    return _descriptorSet;
}

void DescriptorSetWriter::clear() {
    _writes.clear();
    _bufferInfos.clear();
    _imageInfos.clear();
}