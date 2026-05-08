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
            _globalDescriptorSets.reserve(_maxFramesInFlight);
            _perFrameDescriptorSets.reserve(_maxFramesInFlight);
        };

        ~DescriptorSetManager() {};

        DescriptorSetManager(const DescriptorSetManager&) = delete;
        DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
        DescriptorSetManager(DescriptorSetManager&& other) = delete;
        DescriptorSetManager& operator=(DescriptorSetManager&& other) = delete;

        bool setup();

        void cleanup();

        std::optional<DescriptorSetWriter> getDescriptorSetWriter(
            uint32_t frameIndex, DescriptorSetLayout::SetType setType
        );

        const DescriptorSetLayout* getDescriptorSetLayout(DescriptorSetLayout::SetType setType) const;

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DescriptorSetManager");
        std::reference_wrapper<const vax::vk::Device> _device;
        const int32_t _maxFramesInFlight;

        std::optional<DescriptorSetLayout> _globalDescriptorSetLayout = std::nullopt;
        std::optional<DescriptorSetLayout> _perFrameDescriptorSetLayout = std::nullopt;

        VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

        std::vector<VkDescriptorSet> _globalDescriptorSets;
        std::vector<VkDescriptorSet> _perFrameDescriptorSets;

        bool createDescriptorSetLayouts();
        bool createDescriptorSetPool();
    };
}
