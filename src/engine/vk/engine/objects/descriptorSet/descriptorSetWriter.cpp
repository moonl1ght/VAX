#include "descriptorSetWriter.h"
#include "buffer.h"
#include "texture.h"
#include "vkUtils.h"

using namespace vax::vk;
using namespace vax;

void DescriptorSetWriter::writeBuffer(Buffer* buffer, uint32_t binding, uint32_t offset) {
    VkDescriptorBufferInfo& bufferInfo = _bufferInfos.emplace_back(
        VkDescriptorBufferInfo{
            .buffer = buffer->getVkBuffer(),
            .offset = offset,
            .range = buffer->getSize()
        }
    );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    _writes.push_back(write);
}

void DescriptorSetWriter::writeTexture(textures::Texture* texture, uint32_t binding, uint32_t offset) {
    VkDescriptorImageInfo& imageInfo = _imageInfos.emplace_back(
        VkDescriptorImageInfo{
            .sampler = texture->sampler == nullptr ? VK_NULL_HANDLE : texture->sampler->vkSampler,
            .imageView = texture->textureImageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        }
        );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo
    };

    _writes.push_back(write);
}

void DescriptorSetWriter::writeStorageImage(VkImageView imageView, uint32_t binding) {
    VkDescriptorImageInfo& imageInfo = _imageInfos.emplace_back(
        VkDescriptorImageInfo{
            .sampler = VK_NULL_HANDLE,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        }
        );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &imageInfo
    };

    _writes.push_back(write);
}

VkDescriptorSet DescriptorSetWriter::update() {
    for (auto& write : _writes) {
        write.dstSet = _descriptorSet;
    }

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
    _imageInfos.clear();
    _bufferInfos.clear();
}