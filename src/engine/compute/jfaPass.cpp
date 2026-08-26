#include "jfaPass.h"
#include "descriptorSetLayoutBuilder.h"
#include "jfa.h"
#include "pipelineBuilder.h"
#include "shaderModuleBuilder.h"
#include "textureFactory.h"
#include <vulkan/vulkan_core.h>

using namespace vax::engine;
using namespace vax::vk;

void JFAPass::cleanup() {
    if (_initPipeline) {
        vkDestroyPipeline(_device.get().vkDevice, _initPipeline->vkPipeline, nullptr);
    }

    if (_jfaPipeline) {
        vkDestroyPipeline(_device.get().vkDevice, _jfaPipeline->vkPipeline, nullptr);
    }
}

void JFAPass::setup(const std::vector<vax::vk::Texture>& maskTextures, const vax::vk::Texture& depthTexture) {
    if (maskTextures.size() != vax::vk::MAX_FRAMES_IN_FLIGHT) {
        _logger.error("Mask textures are empty!");
        return;
    }

    DescriptorSetLayoutBuilder jfaInitDescriptorSetLayoutBuilder(_device, "jfa_init");
    jfaInitDescriptorSetLayoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
    jfaInitDescriptorSetLayoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
    jfaInitDescriptorSetLayoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
    auto jfaInitLayout = jfaInitDescriptorSetLayoutBuilder.build(DescriptorSetLayout::SetType::OTHER);
    if (!jfaInitLayout) {
        _logger.error("Failed to create init JFA descriptor set layout!");
        return;
    }

    DescriptorSetLayoutBuilder descriptorSetLayoutBuilder(_device, "jfa");
    descriptorSetLayoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
    descriptorSetLayoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
    auto layout = descriptorSetLayoutBuilder.build(DescriptorSetLayout::SetType::OTHER);
    if (!layout) {
        _logger.error("Failed to create init JFA descriptor set layout!");
        return;
    }

    _descriptorSetManager.get().addDescriptorSetLayout("jfa_init", std::move(*jfaInitLayout));
    _descriptorSetManager.get().addDescriptorSetLayout("jfa", std::move(*layout));

    auto initDescriptorSetLayout = _descriptorSetManager.get().getDescriptorSetLayout("jfa_init");
    if (!initDescriptorSetLayout) {
        _logger.error("Failed to get init JFA descriptor set layout!");
        return;
    }

    auto descriptorSetLayout = _descriptorSetManager.get().getDescriptorSetLayout("jfa");
    if (!descriptorSetLayout) {
        _logger.error("Failed to get init JFA descriptor set layout!");
        return;
    }
    VkDescriptorSetLayout initLayouts[] = {initDescriptorSetLayout->getVkDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo initPipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = initLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    ComputePipelineBuilder pipelineBuilder(_device);
    pipelineBuilder.setPipelineLayout(initPipelineLayoutInfo);
    ShaderModuleBuilder shaderModuleBuilder(_device);
    auto initShaderModule = shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/jfa_init.comp.spv"));
    if (!initShaderModule) {
        _logger.error("Failed to create init JFA shader module!");
        return;
    }
    pipelineBuilder.setShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, initShaderModule.value(), "main");
    _initPipeline = pipelineBuilder.build("init_jfa");
    if (!_initPipeline) {
        _logger.error("Failed to create init JFA pipeline!");
        return;
    }
    vkDestroyShaderModule(_device.get().vkDevice, initShaderModule.value(), nullptr);

    VkPushConstantRange pushConstantRange{
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(JFAPushConstants),
    };

    VkDescriptorSetLayout layouts[] = {descriptorSetLayout->getVkDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo jfaPipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = layouts,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange,
    };

    pipelineBuilder.updatePipelineLayout(jfaPipelineLayoutInfo);
    auto jfaShaderModule = shaderModuleBuilder.build(SRC_PATH("engine/shaders/out/jfa.comp.spv"));
    if (!jfaShaderModule) {
        _logger.error("Failed to create JFA shader module!");
        return;
    }
    pipelineBuilder.setShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, jfaShaderModule.value(), "main");
    _jfaPipeline = pipelineBuilder.build("jfa");
    if (!_jfaPipeline) {
        _logger.error("Failed to create JFA pipeline!");
        return;
    }
    vkDestroyShaderModule(_device.get().vkDevice, jfaShaderModule.value(), nullptr);

    writeTextures(maskTextures, depthTexture);
}

void JFAPass::writeTextures(const std::vector<vax::vk::Texture>& maskTextures, const vax::vk::Texture& depthTexture) {
    _jfaTexturesA.clear();
    _jfaTexturesB.clear();
    auto textureFactory = TextureFactory(_device.get(), _allocator);
    for (int i = 0; i < vax::vk::MAX_FRAMES_IN_FLIGHT; ++i) {
        auto initDescriptorSetHandler = _descriptorSetManager.get().createDescriptorSetHandler(
            i, DescriptorSetManager::PoolType::PROCESSING, "jfa_init", "init_jfa", true
        );
        auto jfaDescriptorSetHandler0 = _descriptorSetManager.get().createDescriptorSetHandler(
            i, DescriptorSetManager::PoolType::PROCESSING, "jfa", "jfa_set0", true
        );
        auto jfaDescriptorSetHandler1 = _descriptorSetManager.get().createDescriptorSetHandler(
            i, DescriptorSetManager::PoolType::PROCESSING, "jfa", "jfa_set1", true
        );
        if (!initDescriptorSetHandler || !jfaDescriptorSetHandler0 || !jfaDescriptorSetHandler1) {
            _logger.error("Failed to get JFA descriptor set handlers!");
            return;
        }

        initDescriptorSetHandler->writeTexture(maskTextures[i], 0);
        initDescriptorSetHandler->writeTexture(depthTexture, 1);

        auto inputTextureSize = maskTextures[i].size();
        auto textureA = textureFactory.makeTextureDetached(
            TextureFactory::TextureCreateInfo{
            .name = "jfa_texture_a_" + std::to_string(i),
            .format = VK_FORMAT_R16G16_UINT,
            .size = inputTextureSize,
            .imageUsageFlags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .flags = 0,
            }
        );
        if (!textureA) {
            _logger.error("Failed to create JFA texture A!");
            return;
        }
        textureA->loadImageView(VK_IMAGE_VIEW_TYPE_2D, 1, 1);
        _jfaTexturesA.push_back(std::move(*textureA));
        initDescriptorSetHandler->writeTexture(_jfaTexturesA[i], 2);
        jfaDescriptorSetHandler0->writeTexture(_jfaTexturesA[i], 0);
        jfaDescriptorSetHandler1->writeTexture(_jfaTexturesA[i], 1);

        auto textureB = textureFactory.makeTextureDetached(
            TextureFactory::TextureCreateInfo{
            .name = "jfa_texture_b_" + std::to_string(i),
            .format = VK_FORMAT_R16G16_UINT,
            .size = inputTextureSize,
            .imageUsageFlags = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .flags = 0,
            }
        );
        if (!textureB) {
            _logger.error("Failed to create JFA texture A!");
            return;
        }
        textureB->loadImageView(VK_IMAGE_VIEW_TYPE_2D, 1, 1);
        _jfaTexturesB.push_back(std::move(*textureB));
        jfaDescriptorSetHandler0->writeTexture(_jfaTexturesB[i], 1);
        jfaDescriptorSetHandler1->writeTexture(_jfaTexturesB[i], 0);

        jfaDescriptorSetHandler0->update();
        jfaDescriptorSetHandler1->update();
        initDescriptorSetHandler->update();
    }
}

void JFAPass::execute(
    const VkCommandBuffer& commandBuffer, const vax::vk::Texture& inputTexture, uint32_t currentFrame
) {

    if (!_initPipeline.has_value() && !_jfaPipeline.has_value()) {
        _logger.error("JFA pipelines are not initialized!");
        return;
    }

    VkImageMemoryBarrier2 textureABarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .image = _jfaTexturesA[currentFrame].image(),
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    VkDependencyInfo dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &textureABarrier,
    };

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _initPipeline->vkPipeline);

    auto descriptorSetHandler = _descriptorSetManager.get().createDescriptorSetHandler(
        currentFrame, DescriptorSetManager::PoolType::PROCESSING, "jfa_init", "init_jfa", false
    );
    if (!descriptorSetHandler) {
        _logger.error("Failed to get init JFA descriptor set handler!");
        return;
    }
    descriptorSetHandler->bind(commandBuffer, _initPipeline->vkPipelineLayout, 0, VK_PIPELINE_BIND_POINT_COMPUTE);

    uint32_t width = _jfaTexturesA[currentFrame].width();
    uint32_t height = _jfaTexturesA[currentFrame].height();

    uint32_t groupCountX = (width + 15) / 16;
    uint32_t groupCountY = (height + 15) / 16;
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);

    VkImageMemoryBarrier2 prepareBarriers[2]{
        {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .image = _jfaTexturesA[currentFrame].image(),
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        },
        {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_GENERAL,
        .image = _jfaTexturesB[currentFrame].image(),
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        }
    };

    VkDependencyInfo prepareDependency{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 2,
        .pImageMemoryBarriers = prepareBarriers,
    };
    vkCmdPipelineBarrier2(commandBuffer, &prepareDependency);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _jfaPipeline->vkPipeline);

    bool isTextureAInput = true;
    uint32_t maxDim = std::max(width, height);

    for (uint32_t step = maxDim / 2; step > 0; step /= 2) {

        JFAPushConstants pushConstants{step};

        vkCmdPushConstants(
            commandBuffer,
            _jfaPipeline->vkPipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            sizeof(pushConstants),
            &pushConstants
        );

        std::string setName = isTextureAInput ? "jfa_set0" : "jfa_set1";
        auto jfaDescriptorSetHandler = _descriptorSetManager.get().createDescriptorSetHandler(
            currentFrame, DescriptorSetManager::PoolType::PROCESSING, "jfa", setName, false
        );

        if (!jfaDescriptorSetHandler) {
            _logger.error("Failed to get JFA descriptor set handler for " + setName);
            return;
        }
        jfaDescriptorSetHandler->bind(commandBuffer, _jfaPipeline->vkPipelineLayout, 0, VK_PIPELINE_BIND_POINT_COMPUTE);

        vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);

        VkImageMemoryBarrier2 loopBarriers[2]{
            {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .image = isTextureAInput ? _jfaTexturesB[currentFrame].image() : _jfaTexturesA[currentFrame].image(),
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            },
            {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
            .image = isTextureAInput ? _jfaTexturesA[currentFrame].image() : _jfaTexturesB[currentFrame].image(),
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            }
        };

        VkDependencyInfo loopDependency{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = loopBarriers,
        };
        vkCmdPipelineBarrier2(commandBuffer, &loopDependency);

        isTextureAInput = !isTextureAInput;
    }

    VkImage finalImage = isTextureAInput ? _jfaTexturesA[currentFrame].image() : _jfaTexturesB[currentFrame].image();

    VkImageMemoryBarrier2 finalBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .image = finalImage,
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    VkDependencyInfo finalDependency{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &finalBarrier,
    };
    vkCmdPipelineBarrier2(commandBuffer, &finalDependency);
    _isFinalImageA = isTextureAInput;
}

const std::vector<vax::vk::Texture>& JFAPass::outputATextures() const { return _jfaTexturesA; }

const std::vector<vax::vk::Texture>& JFAPass::outputBTextures() const { return _jfaTexturesB; }