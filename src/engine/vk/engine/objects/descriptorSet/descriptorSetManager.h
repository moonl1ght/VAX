#pragma once

#include "luna.h"
#include "device.h"
#include "descriptorSetLayout.h"
#include "descriptorSetWriter.h"

namespace vax::vk {
    class DescriptorSetManager {
    public:
        explicit DescriptorSetManager(
            const vax::vk::Device& device,
            const int32_t maxFramesInFlight
        )
            : _device(device)
            , _maxFramesInFlight(maxFramesInFlight) {
            _defaultDescriptorSets.resize(1);
        };

        ~DescriptorSetManager() {};

        DescriptorSetManager(const DescriptorSetManager&) = delete;
        DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
        DescriptorSetManager(DescriptorSetManager&& other) = delete;
        DescriptorSetManager& operator=(DescriptorSetManager&& other) = delete;

        bool setup();

        void cleanup();

        std::optional<DescriptorSetWriter> getDefaultDescriptorSetWriter(
            uint32_t frameIndex, DescriptorSetLayout::DefaultType type
        );

        const DescriptorSetLayout& getDefaultDescriptorSetLayout(DescriptorSetLayout::DefaultType type) const {
            return _defaultDescriptorSetLayouts[static_cast<size_t>(type)];
        }

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DescriptorSetManager");
        std::reference_wrapper<const vax::vk::Device> _device;
        const int32_t _maxFramesInFlight;

        std::vector<DescriptorSetLayout> _defaultDescriptorSetLayouts;

        VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

        std::vector<std::vector<VkDescriptorSet>> _defaultDescriptorSets;

        bool createDefaultDescriptorSetLayouts();
        bool createDescriptorSetPool();
    };
}
