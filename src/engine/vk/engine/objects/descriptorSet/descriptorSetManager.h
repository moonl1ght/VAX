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

    enum class SetLayoutName : uint32_t {
        GLOBAL = 0,
        PER_FRAME = 1,
        FINAL_BLEND = 2,        // Inlcudes depth, color textures and mask texture
        FINAL_BLEND_SIMPLE = 3, // Includes only color texture
        SINGLE_STORAGE_IMAGE = 4,
        UNKNOWN = 0xFFFFFFFF,
    };

    static std::string getSetLayoutName(SetLayoutName setLayoutName) {
        switch (setLayoutName) {
        case SetLayoutName::GLOBAL:
            return "global";
        case SetLayoutName::PER_FRAME:
            return "per_frame";
        case SetLayoutName::FINAL_BLEND:
            return "final_blend";
        case SetLayoutName::FINAL_BLEND_SIMPLE:
            return "final_blend_simple";
        case SetLayoutName::SINGLE_STORAGE_IMAGE:
            return "single_storage_image";
        case SetLayoutName::UNKNOWN:
            return "unknown";
        default:
            return "unknown";
        }
    }

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

    std::optional<DescriptorSetHandler> createDefaultDescriptorSetHandler(
        uint32_t frameIndex, PoolType poolType, SetLayoutName setLayoutName, std::string name, bool autoCreate
    );

    std::optional<DescriptorSetHandler> createDescriptorSetHandler(
      uint32_t frameIndex, PoolType poolType, std::string setLayoutName, std::string name, bool autoCreate
  );

    const DescriptorSetLayout* getDescriptorSetLayout(std::string name) const;

    const DescriptorSetLayout* getDefaultDescriptorSetLayout(SetLayoutName setLayoutName) const;

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
