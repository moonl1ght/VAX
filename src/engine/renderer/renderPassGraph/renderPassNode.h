#pragma once

#include <string>

#include "commandBuffer.h"
#include "drawableScene.h"

namespace vax::engine {
class RenderPassNode {
  public:
    struct InputDescriptorSetInfo {
        vax::vk::DescriptorSetManager::PoolType poolType;
        vax::vk::DescriptorSetManager::SetLayoutName layoutName;
        std::string name;

        struct BindingInfo {
            uint32_t setIndex;
            VkPipelineBindPoint bindPoint;
            uint32_t dynamicOffsetCount;
            std::vector<uint32_t> dynamicOffsets;
        };

        BindingInfo bindingInfo;
    };

    struct RunPassInfo final {
        vax::vk::CommandBuffer& commandBuffer;
        vax::engine::DrawableScene* scene;
        uint32_t imageIndex;
        uint32_t frameIndex;
    };

    RenderPassNode* next = nullptr;
    RenderPassNode* prev = nullptr;

    RenderPassNode(std::string_view id)
        : _id(id) {};

    RenderPassNode(const RenderPassNode&) = delete;
    RenderPassNode(RenderPassNode&&) = default;
    RenderPassNode& operator=(const RenderPassNode&) = delete;
    RenderPassNode& operator=(RenderPassNode&&) = default;

    virtual ~RenderPassNode() = default;

    virtual void execute(RunPassInfo& runPassInfo) final {
      if (_enabled) {
        runPass(runPassInfo);
      }
      if (next) {
        next->execute(runPassInfo);
      }
    }

    void setEnabled(bool enabled) { _enabled = enabled; }

    virtual void runPass(RunPassInfo& runPassInfo) = 0;

  protected:
    std::string _id;
    bool _enabled = true;
};
} // namespace vax::engine