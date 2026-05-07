#pragma once

#include <deque>
#include "luna.h"
#include "device.h"

namespace vax::vk {
    class Buffer;
}

namespace vax::textures {
    class Texture;
}

namespace vax::vk {
    struct DescriptorSetWriter {
    public:
        explicit DescriptorSetWriter(
            const vax::vk::Device& device,
            VkDescriptorSet descriptorSet
        )
            : _device(device)
            , _descriptorSet(descriptorSet) {
        }

        ~DescriptorSetWriter() {}

        void writeBuffer(
            const Buffer& buffer,
            uint32_t binding,
            uint32_t offset,
            VkDescriptorType descriptorType,
            uint32_t descriptorCount = 1
        );

        void writeTexture(
            const vax::textures::Texture& texture,
            uint32_t binding,
            VkDescriptorType descriptorType,
            uint32_t descriptorCount
        );

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
    };
}