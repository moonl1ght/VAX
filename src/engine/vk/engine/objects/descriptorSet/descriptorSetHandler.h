#pragma once

#include "device.h"
#include "luna.h"
#include <deque>

namespace vax::vk {
class Buffer;
class Texture;
class Sampler;
} // namespace vax::vk

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

    void writeBuffer(
        const Buffer& buffer,
        uint32_t binding,
        uint32_t offset,
        VkDescriptorType descriptorType,
        uint32_t arrayElement = 0
    );

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