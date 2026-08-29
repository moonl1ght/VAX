#include "renderPassGraphFactory.h"
#include "jfaPass.h"
#include "renderDestinationBuilder.h"
#include "renderPassDescriptorBuilder.h"
#include "renderPass_V2.h"

using namespace vax::engine;
using namespace vax::vk;
using namespace vax;

void RenderPassGraphFactory::setupRenderPassDescriptors(VkFormat imageFormat) {
    auto renderPassDescriptorBuilder = RenderPassDescriptorBuilder(_device.get());
    auto mainRenderPassDescriptor = renderPassDescriptorBuilder.buildOffscreen(imageFormat, false);
    auto shadowSunRenderPassDescriptor = renderPassDescriptorBuilder.buildShadowSun(imageFormat, false);
    auto swapchainRenderPassDescriptor = renderPassDescriptorBuilder.buildSwapchain(imageFormat);
    if (!mainRenderPassDescriptor.has_value() || !shadowSunRenderPassDescriptor.has_value() ||
        !swapchainRenderPassDescriptor.has_value()) {
        _logger.error("Failed to create main render pass descriptor!");
        return;
    }

    auto mainRenderPassDescriptorShared =
        std::make_shared<vax::vk::RenderPassDescriptor>(std::move(mainRenderPassDescriptor.value()));
    auto shadowSunRenderPassDescriptorShared =
        std::make_shared<vax::vk::RenderPassDescriptor>(std::move(shadowSunRenderPassDescriptor.value()));
    auto swapchainRenderPassDescriptorShared =
        std::make_shared<vax::vk::RenderPassDescriptor>(std::move(swapchainRenderPassDescriptor.value()));

    _pipelineManager.get().setup(
        *mainRenderPassDescriptorShared, *swapchainRenderPassDescriptorShared, *shadowSunRenderPassDescriptorShared
    );

    _uiEngine.get().setup(swapchainRenderPassDescriptorShared->getVkRenderPass());

    _renderPassDescriptors.emplace("main", mainRenderPassDescriptorShared);
    _renderPassDescriptors.emplace("shadow_sun", shadowSunRenderPassDescriptorShared);
    _renderPassDescriptors.emplace("swapchain", swapchainRenderPassDescriptorShared);
}

void RenderPassGraphFactory::setupRenderDestinations(
    vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
) {
    auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");
    auto& shadowSunRenderPassDescriptor = _renderPassDescriptors.at("shadow_sun");

    auto renderDestinationBuilder = RenderDestinationBuilder(_device.get(), _allocator);
    auto mainRenderDestination = renderDestinationBuilder.buildMainOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, *mainRenderPassDescriptor
    );
    if (!mainRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return;
    }

    auto swapchainRenderDestination = renderDestinationBuilder.buildSwapchain(
        commandManager, queueManager.graphicsQueue, swapchain, *swapchainRenderPassDescriptor
    );
    if (!swapchainRenderDestination.has_value()) {
        _logger.error("Failed to create swapchain render destination!");
        return;
    }

    auto shadowSunRenderDestination = renderDestinationBuilder.buildShadowSunOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, *shadowSunRenderPassDescriptor
    );
    if (!shadowSunRenderDestination.has_value()) {
        _logger.error("Failed to create shadow sun render destination!");
        return;
    }

    auto mainRenderDestinationShared =
        std::make_shared<vax::vk::RenderDestination>(std::move(mainRenderDestination.value()));
    auto swapchainRenderDestinationShared =
        std::make_shared<vax::vk::RenderDestination>(std::move(swapchainRenderDestination.value()));
    auto shadowSunRenderDestinationShared =
        std::make_shared<vax::vk::RenderDestination>(std::move(shadowSunRenderDestination.value()));

    _renderDestinations.emplace("main", mainRenderDestinationShared);
    _renderDestinations.emplace("swapchain", swapchainRenderDestinationShared);
    _renderDestinations.emplace("shadow_sun", shadowSunRenderDestinationShared);
}

void RenderPassGraphFactory::setupRenderDestinationsForRoverCamera(
    vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
) {
    auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");

    auto renderDestinationBuilder = RenderDestinationBuilder(_device.get(), _allocator);

    auto roverCameraRenderDestination = renderDestinationBuilder.buildMainOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, *mainRenderPassDescriptor
    );
    if (!roverCameraRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return;
    }

    auto roverCameraFBRenderDestination = renderDestinationBuilder.buildSwapchain(
        commandManager, queueManager.graphicsQueue, swapchain, *swapchainRenderPassDescriptor
    );
    if (!roverCameraFBRenderDestination.has_value()) {
        _logger.error("Failed to create rover camera swapchain render destination!");
        return;
    }

    auto roverCameraRenderDestinationShared =
        std::make_shared<vax::vk::RenderDestination>(std::move(roverCameraRenderDestination.value()));
    auto roverCameraFBRenderDestinationShared =
        std::make_shared<vax::vk::RenderDestination>(std::move(roverCameraFBRenderDestination.value()));

    _renderDestinations.emplace("rover_camera_main", roverCameraRenderDestinationShared);
    _renderDestinations.emplace("rover_camera_swapchain", roverCameraFBRenderDestinationShared);
}

std::unique_ptr<RenderPassGraph> RenderPassGraphFactory::buildRoverDemoGraph() {
    auto swapchain = _windowController.get().getWindow(0)->getSwapchain();

    auto shadowPass = std::make_shared<RenderPass_V2>(
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
    shadowPass->setDrawWork([](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
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

    auto mainPass = std::make_shared<RenderPass_V2>(
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
    mainPass->setDrawWork([](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->draw(drawContext);
    });

    auto jfaPass = std::make_shared<JFAPass>("jfa_pass", _device.get(), _descriptorSetManager.get(), _allocator);
    jfaPass->setup(_renderDestinations.at("main"));

    jfaPass->setPostPassWork([](RenderPassNode* passNode, RenderPass_V2::RunPassInfo& runPassInfo) {
        auto jfaPass = dynamic_cast<JFAPass*>(passNode);
        if (!jfaPass) {
            return;
        }
        if (jfaPass->next) {
            auto finalBlendPass = dynamic_cast<RenderPass_V2*>(jfaPass->next.get());
            if (!finalBlendPass) {
                return;
            }
            auto descriptorSetInfo = finalBlendPass->getInputDescriptorSetAt(1);
            descriptorSetInfo.name = jfaPass->outputDescriptorSetName();
            finalBlendPass->updateInputDescriptorSetAt(1, descriptorSetInfo);
        }
    });

    auto finalBlendPass = std::make_shared<RenderPass_V2>(
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
    finalBlendPass->setDrawWork([&uiEngine = _uiEngine.get()](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->drawBackground(drawContext);
        uiEngine.render(runPassInfo.commandBuffer.vkCommandBuffer);
    });

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