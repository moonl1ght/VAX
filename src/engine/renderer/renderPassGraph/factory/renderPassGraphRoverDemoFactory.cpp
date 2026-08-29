#include "renderPassGraphFactory.h"
#include "renderPass.h"
#include "jfaPass.h"

using namespace vax::engine;
using namespace vax::vk;

std::unique_ptr<RenderPassGraph> RenderPassGraphFactory::buildRoverDemoGraph() {
    auto swapchain = _windowController.get().getWindow(0)->getSwapchain();

    auto shadowPass = std::make_shared<RenderPass>(
        "shadow_pass",
        _device.get(),
        _pipelineManager.get(),
        _descriptorSetManager.get(),
        "ShadowPass",
        _renderDestinations.at("shadow_sun"),
        _renderPassDescriptors.at("shadow_sun")
    );
    shadowPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = swapchain->swapchainExtent});
    shadowPass->setPipeline(vax::vk::PipelineName::SHADOW);
    shadowPass->setPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    shadowPass->setDrawWork([](RenderPass::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->draw(drawContext);
    });
    auto dynamicOffset = static_cast<uint32_t>(_device.get().minUniformBufferOffsetAlignment<UniformBufferObject>());
    shadowPass->addInputDescriptorSet({
        .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
        .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
        .name = "per_frame",
        .bindingInfo = {
        .setIndex = MainSetIndices::PER_FRAME_SET_INDEX,
        .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .dynamicOffsetCount = 1,
        .dynamicOffsets = {dynamicOffset},
        },
    });
    shadowPass->setSwapchainExtent(swapchain->swapchainExtent);

    auto mainPass = std::make_shared<RenderPass>(
        "main_pass",
        _device.get(),
        _pipelineManager.get(),
        _descriptorSetManager.get(),
        "MainPass",
        _renderDestinations.at("main"),
        _renderPassDescriptors.at("main")
    );
    mainPass->addInputDescriptorSet({
        .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
        .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
        .name = "per_frame",
        .bindingInfo = {
        .setIndex = MainSetIndices::PER_FRAME_SET_INDEX,
        .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .dynamicOffsetCount = 1,
        .dynamicOffsets = {0},
        },
    });
    mainPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = swapchain->swapchainExtent});
    mainPass->setSwapchainExtent(swapchain->swapchainExtent);
    mainPass->setPipeline(vax::vk::PipelineName::PBR);
    mainPass->setPipelineLayout(vax::vk::PipelineLayoutName::BASE);
    mainPass->setDrawWork([](RenderPass::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->draw(drawContext);
    });

    auto jfaPass = std::make_shared<JFAPass>("jfa_pass", _device.get(), _descriptorSetManager.get(), _allocator);
    jfaPass->setup(_renderDestinations.at("main"));

    jfaPass->setPostPassWork([](RenderPassNode* passNode, RenderPass::RunPassInfo& runPassInfo) {
        auto jfaPass = dynamic_cast<JFAPass*>(passNode);
        if (!jfaPass) {
            return;
        }
        if (jfaPass->next) {
            auto finalBlendPass = dynamic_cast<RenderPass*>(jfaPass->next.get());
            if (!finalBlendPass) {
                return;
            }
            auto descriptorSetInfo = finalBlendPass->getInputDescriptorSetAt(1);
            descriptorSetInfo.name = jfaPass->outputDescriptorSetName();
            finalBlendPass->updateInputDescriptorSetAt(1, descriptorSetInfo);
        }
    });

    auto finalBlendPass = std::make_shared<RenderPass>(
        "final_blend_pass",
        _device.get(),
        _pipelineManager.get(),
        _descriptorSetManager.get(),
        "FinalBlendPass",
        _renderDestinations.at("swapchain"),
        _renderPassDescriptors.at("swapchain")
    );
    finalBlendPass->addInputDescriptorSet({
        .poolType = vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
        .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::FINAL_BLEND,
        .name = "final_blend",
        .bindingInfo = {
        .setIndex = 0,
        .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .dynamicOffsetCount = 1,
        .dynamicOffsets = {0},
        },
    });
    finalBlendPass->addInputDescriptorSet({
        .poolType = vax::vk::DescriptorSetManager::PoolType::FINAL_BLEND,
        .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::SINGLE_STORAGE_IMAGE,
        .name = jfaPass->outputDescriptorSetName(),
        .bindingInfo = {
        .setIndex = 1,
        .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .dynamicOffsetCount = 1,
        .dynamicOffsets = {0},
        },
    });
    finalBlendPass->addInputDescriptorSet({
        .poolType = vax::vk::DescriptorSetManager::PoolType::PER_FRAME,
        .layoutName = vax::vk::DescriptorSetManager::SetLayoutName::PER_FRAME,
        .name = "per_frame",
        .bindingInfo = {
        .setIndex = 2,
        .bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .dynamicOffsetCount = 1,
        .dynamicOffsets = {0},
        },
    });
    finalBlendPass->setRenderToSwapchain(true);
    finalBlendPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = swapchain->swapchainExtent});
    finalBlendPass->setSwapchainExtent(swapchain->swapchainExtent);
    finalBlendPass->setPipeline(vax::vk::PipelineName::FINAL_BLEND);
    finalBlendPass->setPipelineLayout(vax::vk::PipelineLayoutName::FINAL_BLEND);
    finalBlendPass->setDrawWork(
        [&uiEngine = _uiEngine.get()](RenderPass::RunPassInfo& runPassInfo, DrawContext& drawContext) {
            runPassInfo.scene->drawBackground(drawContext);
            uiEngine.render(runPassInfo.commandBuffer.vkCommandBuffer);
        }
    );

    shadowPass->next = mainPass;

    mainPass->prev = shadowPass;
    mainPass->next = jfaPass;

    jfaPass->prev = mainPass;
    jfaPass->next = finalBlendPass;

    finalBlendPass->prev = jfaPass;

    auto renderPassGraph =
        std::make_unique<RenderPassGraph>(shadowPass, _renderPassDescriptors, _renderDestinations, _uiEngine.get());
    return renderPassGraph;
}