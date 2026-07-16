#pragma once
#include "descriptorSetManager.h"
#include "pipeline.h"
#include "texture.h"
#include "textureManager.h"

namespace vax::engine {
class JFAPass final {
  public:
    JFAPass(
        const vax::vk::Device& device,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        VmaAllocator allocator
    )
        : _device(device)
        , _descriptorSetManager(descriptorSetManager)
        , _allocator(allocator) {};

    ~JFAPass() = default;

    JFAPass(const JFAPass& other) = delete;
    JFAPass& operator=(const JFAPass& other) = delete;
    JFAPass(JFAPass&& other) noexcept = default;
    JFAPass& operator=(JFAPass&& other) noexcept = default;

    void setup(const std::vector<vax::vk::Texture>& maskTextures, const vax::vk::Texture& depthTexture);

    void cleanup();

    void execute(const VkCommandBuffer& commandBuffer, const vax::vk::Texture& inputTexture, uint32_t currentFrame);

    const std::vector<vax::vk::Texture>& outputATextures() const;

    const std::vector<vax::vk::Texture>& outputBTextures() const;

    bool isFinalImageA() const { return _isFinalImageA; }

    void writeTextures(const std::vector<vax::vk::Texture>& maskTextures, const vax::vk::Texture& depthTexture);

  private:
    vax::Logger _logger = vax::Logger("JFAPass");
    std::reference_wrapper<const vax::vk::Device> _device;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;
    VmaAllocator _allocator;
    std::optional<vax::vk::Pipeline> _initPipeline;
    std::optional<vax::vk::Pipeline> _jfaPipeline;
    std::vector<vax::vk::Texture> _jfaTexturesA;
    std::vector<vax::vk::Texture> _jfaTexturesB;
    bool _isFinalImageA = true;
};
} // namespace vax::engine