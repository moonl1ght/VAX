#pragma once

#include <deque>
#include "luna.h"
#include "device.h"

namespace vax::vk {
    class Buffer;
}

namespace vax::textures {
    class Texture;
    class Sampler;
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
            _writes.reserve(100);
        }

        ~DescriptorSetWriter() {}

        DescriptorSetWriter(const DescriptorSetWriter&) = delete;
        DescriptorSetWriter& operator=(const DescriptorSetWriter&) = delete;
        DescriptorSetWriter(DescriptorSetWriter&& other) noexcept = delete;
        DescriptorSetWriter& operator=(DescriptorSetWriter&& other) noexcept = delete;

        void writeBuffer(
            const Buffer& buffer,
            uint32_t binding,
            uint32_t offset,
            VkDescriptorType descriptorType,
            uint32_t arrayElement = 0
        );

        void writeTexture(
            const vax::textures::Texture& texture,
            uint32_t binding,
            bool useSampler,
            uint32_t arrayElement = 0
        );

        void writeTextures(
            const std::vector<const vax::textures::Texture*>& textures,
            uint32_t binding,
            bool useSampler
        );

        void writeSampler(
            const vax::textures::Sampler& sampler,
            uint32_t binding,
            uint32_t arrayElement = 0
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
        std::deque<std::vector<VkDescriptorImageInfo>> _imageInfosArray;
    };
}