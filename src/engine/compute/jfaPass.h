#pragma once
#include "descriptorSetManager.h"
#include "pipeline.h"
#include "renderDestination.h"
#include "renderPassNode.h"
#include "texture.h"

namespace vax::engine {
class JFAPass final : public RenderPassNode {
  public:
    JFAPass(
        std::string_view id,
        const vax::vk::Device& device,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        VmaAllocator allocator
    )
        : RenderPassNode(id)
        , _device(device)
        , _descriptorSetManager(descriptorSetManager)
        , _allocator(allocator) {};

    ~JFAPass() {
        cleanup();
    }

    JFAPass(const JFAPass& other) = delete;
    JFAPass& operator=(const JFAPass& other) = delete;
    JFAPass(JFAPass&& other) noexcept = default;
    JFAPass& operator=(JFAPass&& other) noexcept = default;

    void setup(std::weak_ptr<vax::vk::RenderDestination> inputRenderDestination);

    void update(std::weak_ptr<vax::vk::RenderDestination> inputRenderDestination);

    void cleanup();

    const std::vector<vax::vk::Texture>& outputATextures() const;

    const std::vector<vax::vk::Texture>& outputBTextures() const;

    std::string outputDescriptorSetName() const {
        if (_isFinalImageA) {
            return "fb_input_mask_0";
        } else {
            return "fb_input_mask_1";
        }
    }

    void runPass(RunPassInfo& runPassInfo) override;

  private:
    vax::Logger _logger = vax::Logger("JFAPass");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;
    VmaAllocator _allocator;
    std::optional<vax::vk::Pipeline> _initPipeline;
    std::optional<vax::vk::Pipeline> _jfaPipeline;
    std::vector<vax::vk::Texture> _jfaTexturesA;
    std::vector<vax::vk::Texture> _jfaTexturesB;
    std::weak_ptr<vax::vk::RenderDestination> _inputRenderDestination;
    bool _isFinalImageA = true;

    void _writeTextures(const std::vector<vax::vk::Texture>& maskTextures, const vax::vk::Texture& depthTexture);
};
} // namespace vax::engine