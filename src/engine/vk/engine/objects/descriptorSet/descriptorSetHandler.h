#pragma once

#include "buffer.h"
#include "device.h"
#include "luna.h"
#include "sampler.h"
#include "texture.h"
#include <deque>

namespace vax::vk {
/*
 * DescriptorSetHandler is a lightweight wrapper around a VkDescriptorSet.
 * Better not save this object, livetime should be limited to the scope of the function that creates it.
 */
struct DescriptorSetHandler {
  public:
    explicit DescriptorSetHandler(const vax::vk::Device& device, VkDescriptorSet descriptorSet)
        : _device(device)
        , _descriptorSet(descriptorSet) {
        _writes.reserve(100);
    }

    ~DescriptorSetHandler() {}

    DescriptorSetHandler(const DescriptorSetHandler&) = delete;
    DescriptorSetHandler& operator=(const DescriptorSetHandler&) = delete;
    DescriptorSetHandler(DescriptorSetHandler&& other) noexcept = delete;
    DescriptorSetHandler& operator=(DescriptorSetHandler&& other) noexcept = delete;

    template <typename T>
    void writeBuffer(
        const Buffer<T>& buffer,
        uint32_t binding,
        uint32_t offset,
        VkDescriptorType descriptorType,
        uint32_t arrayElement = 0
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

    void writeTexture(const Texture& texture, uint32_t binding, uint32_t arrayElement = 0, bool withSampler = false);

    void writeTextures(const std::vector<const Texture*>& textures, uint32_t binding, bool withSampler = false);

    void writeSampler(const Sampler& sampler, uint32_t binding, uint32_t arrayElement = 0);

    VkDescriptorSet getDescriptorSet() const { return _descriptorSet; }

    VkDescriptorSet update();

    void clear();

    void bind(
        VkCommandBuffer commandBuffer,
        VkPipelineLayout pipelineLayout,
        uint32_t setIndex,
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        uint32_t dynamicOffsetCount = 0,
        const uint32_t* pDynamicOffsets = nullptr
    );

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetWriter");
    std::reference_wrapper<const vax::vk::Device> _device;
    VkDescriptorSet _descriptorSet;
    std::vector<VkWriteDescriptorSet> _writes;
    std::deque<VkDescriptorBufferInfo> _bufferInfos;
    std::deque<VkDescriptorImageInfo> _imageInfos;
    std::deque<std::vector<VkDescriptorImageInfo>> _imageInfosArray;
};
} // namespace vax::vk