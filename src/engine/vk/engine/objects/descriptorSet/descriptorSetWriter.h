#pragma once

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

        void writeBuffer(vax::vk::Buffer* buffer, uint32_t binding, uint32_t offset = 0);
        void writeTexture(vax::textures::Texture* texture, uint32_t binding, uint32_t offset = 0);
        void writeStorageImage(VkImageView imageView, uint32_t binding);

        VkDescriptorSet getDescriptorSet() const { return _descriptorSet; }

        VkDescriptorSet update();

        void clear();

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DescriptorSetWriter");
        std::reference_wrapper<const vax::vk::Device> _device;
        VkDescriptorSet _descriptorSet;
        std::vector<VkWriteDescriptorSet> _writes;
        std::vector<VkDescriptorBufferInfo> _bufferInfos;
        std::vector<VkDescriptorImageInfo> _imageInfos;
    };
}