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
        PER_FRAME = 3,
    };

    explicit DescriptorSetManager(const vax::vk::Device& device, const int32_t maxFramesInFlight)
        : _device(device)
        , _maxFramesInFlight(maxFramesInFlight) {};

    ~DescriptorSetManager() {};

    DescriptorSetManager(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
    DescriptorSetManager(DescriptorSetManager&& other) = delete;
    DescriptorSetManager& operator=(DescriptorSetManager&& other) = delete;

    bool setup();

    void cleanup();

    std::optional<DescriptorSetHandler>
    getDescriptorSetHandler(uint32_t frameIndex, PoolType poolType, std::string name, std::string layoutName);

    const DescriptorSetLayout* getDescriptorSetLayout(std::string name) const;

    void addDescriptorSetLayout(std::string name, DescriptorSetLayout&& layout);

  private:
    vax::Logger _logger = vax::Logger("DescriptorSetManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    const int32_t _maxFramesInFlight;

    std::unordered_map<std::string, DescriptorSetLayout> _descriptorSetLayouts;
    std::unordered_map<std::string, std::vector<VkDescriptorSet>> _descriptorSets;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool _processingDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool _finalBlendDescriptorPool = VK_NULL_HANDLE;

    bool _createDescriptorSetLayouts();
    bool _createDescriptorSetPools();
};
} // namespace vax::vk
