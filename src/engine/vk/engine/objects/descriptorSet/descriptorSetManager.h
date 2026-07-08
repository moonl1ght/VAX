#pragma once

#include "descriptorSetHandler.h"
#include "descriptorSetLayout.h"
#include "device.h"
#include "luna.h"

namespace vax::vk {
class DescriptorSetManager {
  public:
    explicit DescriptorSetManager(const vax::vk::Device& device, const int32_t maxFramesInFlight)
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

    std::optional<DescriptorSetHandler>
    getDescriptorSetHandler(uint32_t frameIndex, DescriptorSetLayout::SetType setType);

    const DescriptorSetLayout* getDescriptorSetLayout(DescriptorSetLayout::SetType setType) const;

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    const int32_t _maxFramesInFlight;

    std::optional<DescriptorSetLayout> _globalDescriptorSetLayout = std::nullopt;
    std::optional<DescriptorSetLayout> _perFrameDescriptorSetLayout = std::nullopt;
    std::optional<DescriptorSetLayout> _finalBlendDescriptorSetLayout = std::nullopt;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool _finalBlendDescriptorPool = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _globalDescriptorSets;
    std::vector<VkDescriptorSet> _perFrameDescriptorSets;
    std::vector<VkDescriptorSet> _finalBlendDescriptorSets;

    bool _createDescriptorSetLayouts();
    bool _createDescriptorSetPools();
};
} // namespace vax::vk
