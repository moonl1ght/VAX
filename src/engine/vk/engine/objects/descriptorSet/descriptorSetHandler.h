#pragma once

#include "device.h"
#include "luna.h"
#include <deque>

namespace vax::vk {
class Buffer;
}

namespace vax::textures {
class Texture;
class Sampler;
} // namespace vax::textures

namespace vax::vk {
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

    void
    writeTexture(const vax::textures::Texture& texture, uint32_t binding, uint32_t arrayElement = 0);

    void writeTextures(const std::vector<const vax::textures::Texture*>& textures, uint32_t binding);

    void writeSampler(const vax::textures::Sampler& sampler, uint32_t binding, uint32_t arrayElement = 0);

    VkDescriptorSet getDescriptorSet() const { return _descriptorSet; }

    VkDescriptorSet update();

    void clear();

  private:
    vax::utils::Logger _logger = vax::utils::Logger("DescriptorSetWriter");
    std::reference_wrapper<const vax::vk::Device> _device;
    VkDescriptorSet _descriptorSet;
    std::vector<VkWriteDescriptorSet> _writes;
    std::deque<VkDescriptorBufferInfo> _bufferInfos;
    std::deque<VkDescriptorImageInfo> _imageInfos;
    std::deque<std::vector<VkDescriptorImageInfo>> _imageInfosArray;
};
} // namespace vax::vk