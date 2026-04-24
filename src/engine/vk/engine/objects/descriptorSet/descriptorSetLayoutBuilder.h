#pragma once

#include "luna.h"
#include "device.h"
#include "descriptorSetLayout.h"

namespace vax::vk {
    class DescriptorSetLayoutBuilder final {
    public:
        explicit DescriptorSetLayoutBuilder(const vax::vk::Device& device) : _device(device) {}

        ~DescriptorSetLayoutBuilder() {}

        DescriptorSetLayoutBuilder(const DescriptorSetLayoutBuilder&) = delete;
        DescriptorSetLayoutBuilder& operator=(const DescriptorSetLayoutBuilder&) = delete;
        DescriptorSetLayoutBuilder(DescriptorSetLayoutBuilder&& other) = delete;
        DescriptorSetLayoutBuilder& operator=(DescriptorSetLayoutBuilder&& other) = delete;

        void addBinding(
            uint32_t binding,
            VkDescriptorType type,
            VkShaderStageFlags stageFlags
        );

        void clear();

        std::optional<DescriptorSetLayout> build(
            DescriptorSetLayout::DefaultType defaultType,
            void* pNext = nullptr,
            VkDescriptorSetLayoutCreateFlags flags = 0
        );

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DescriptorSetLayoutBuilder");
        std::reference_wrapper<const vax::vk::Device> _device;
        std::vector<VkDescriptorSetLayoutBinding> _bindings;
    };
}