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
struct DescriptorSetHandler {
  public:
    explicit DescriptorSetHandler(const vax::vk::Device& device, VkDescriptorSet descriptorSet, uint32_t id)
        : _device(device)
        , _descriptorSet(descriptorSet)
        , _id(id) {
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

    void writeTexture(const Texture& texture, uint32_t binding, uint32_t arrayElement = 0);

    void writeTextures(const std::vector<const Texture*>& textures, uint32_t binding);

    void writeSampler(const Sampler& sampler, uint32_t binding, uint32_t arrayElement = 0);

    VkDescriptorSet getDescriptorSet() const { return _descriptorSet; }

    VkDescriptorSet update();

    void clear();

    uint32_t id() const { return _id; }

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetWriter");
    std::reference_wrapper<const vax::vk::Device> _device;
    uint32_t _id;
    VkDescriptorSet _descriptorSet;
    std::vector<VkWriteDescriptorSet> _writes;
    std::deque<VkDescriptorBufferInfo> _bufferInfos;
    std::deque<VkDescriptorImageInfo> _imageInfos;
    std::deque<std::vector<VkDescriptorImageInfo>> _imageInfosArray;
};
} // namespace vax::vk