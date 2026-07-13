#pragma once

#include "descriptorSetHandler.h"
#include "descriptorSetLayout.h"
#include "device.h"
#include "luna.h"
#include <unordered_map>

namespace vax::vk {
class DescriptorSetManager {
  public:
    enum class PoolType {
        GLOBAL = 0,
        PROCESSING = 1,
        FINAL_BLEND = 2,
    };

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

    std::optional<DescriptorSetHandler>
    getDescriptorSetHandler(uint32_t frameIndex, PoolType poolType, std::string name, std::string layoutName);

    const DescriptorSetLayout* getDescriptorSetLayout(DescriptorSetLayout::SetType setType) const;

    const DescriptorSetLayout* getDescriptorSetLayout(std::string name) const;

    void addDescriptorSetLayout(std::string name, DescriptorSetLayout&& layout);

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    const int32_t _maxFramesInFlight;

    std::optional<DescriptorSetLayout> _globalDescriptorSetLayout = std::nullopt;
    std::optional<DescriptorSetLayout> _perFrameDescriptorSetLayout = std::nullopt;
    std::unordered_map<std::string, DescriptorSetLayout> _descriptorSetLayouts;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool _processingDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool _finalBlendDescriptorPool = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _globalDescriptorSets;
    std::vector<VkDescriptorSet> _perFrameDescriptorSets;
    std::unordered_map<std::string, std::vector<VkDescriptorSet>> _descriptorSets;

    bool _createDescriptorSetLayouts();
    bool _createDescriptorSetPools();
};
} // namespace vax::vk
