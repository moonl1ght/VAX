#include "rednerPassGraphFactory.h"
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

    _pipelineManager.get().setup(
        *mainRenderPassDescriptor, *swapchainRenderPassDescriptor, *shadowSunRenderPassDescriptor
    );

    _renderPassDescriptors.emplace("main", std::move(mainRenderPassDescriptor.value()));
    _renderPassDescriptors.emplace("shadow_sun", std::move(shadowSunRenderPassDescriptor.value()));
    _renderPassDescriptors.emplace("swapchain", std::move(swapchainRenderPassDescriptor.value()));
}

void RenderPassGraphFactory::setupRenderDestinations(
    vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
) {
    auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");
    auto& shadowSunRenderPassDescriptor = _renderPassDescriptors.at("shadow_sun");

    auto renderDestinationBuilder = RenderDestinationBuilder(_device.get(), _allocator);
    auto mainRenderDestination = renderDestinationBuilder.buildMainOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, mainRenderPassDescriptor
    );
    if (!mainRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return;
    }

    auto swapchainRenderDestination = renderDestinationBuilder.buildSwapchain(
        commandManager, queueManager.graphicsQueue, swapchain, swapchainRenderPassDescriptor
    );
    if (!swapchainRenderDestination.has_value()) {
        _logger.error("Failed to create swapchain render destination!");
        return;
    }

    auto shadowSunRenderDestination = renderDestinationBuilder.buildShadowSunOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, shadowSunRenderPassDescriptor
    );
    if (!shadowSunRenderDestination.has_value()) {
        _logger.error("Failed to create shadow sun render destination!");
        return;
    }

    _renderDestinations.emplace("main", std::move(mainRenderDestination.value()));
    _renderDestinations.emplace("swapchain", std::move(swapchainRenderDestination.value()));
    _renderDestinations.emplace("shadow_sun", std::move(shadowSunRenderDestination.value()));
}

void RenderPassGraphFactory::setupRenderDestinationsForRoverCamera(
    vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
) {
    auto& mainRenderPassDescriptor = _renderPassDescriptors.at("main");
    auto& swapchainRenderPassDescriptor = _renderPassDescriptors.at("swapchain");

    auto renderDestinationBuilder = RenderDestinationBuilder(_device.get(), _allocator);

    auto roverCameraRenderDestination = renderDestinationBuilder.buildMainOffscreen(
        commandManager, queueManager.graphicsQueue, swapchain.swapchainExtent, mainRenderPassDescriptor
    );
    if (!roverCameraRenderDestination.has_value()) {
        _logger.error("Failed to create main render destination!");
        return;
    }

    auto roverCameraFBRenderDestination = renderDestinationBuilder.buildSwapchain(
        commandManager, queueManager.graphicsQueue, swapchain, swapchainRenderPassDescriptor
    );
    if (!roverCameraFBRenderDestination.has_value()) {
        _logger.error("Failed to create rover camera swapchain render destination!");
        return;
    }

    _renderDestinations.emplace("rover_camera_main", std::move(roverCameraRenderDestination.value()));
    _renderDestinations.emplace("rover_camera_swapchain", std::move(roverCameraFBRenderDestination.value()));
}

std::unique_ptr<RenderPassGraph> RenderPassGraphFactory::buildRoverDemoGraph() {
    auto swapchain = _windowController.get().getWindow(0)->getSwapchain();
    auto shadowPass = std::make_optional<RenderPass_V2>(
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
    shadowPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->draw(drawContext);
    });
    auto dynamicOffset =
        static_cast<uint32_t>(_device.get().minUniformBufferOffsetAlignment<UniformBufferObject>());
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

    auto mainPass = std::make_optional<RenderPass_V2>(
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
    mainPass->setDrawWork([this](RenderPass_V2::RunPassInfo& runPassInfo, DrawContext& drawContext) {
        runPassInfo.scene->draw(drawContext);
    });

    auto renderPassGraph =
        std::make_unique<RenderPassGraph>(std::move(_renderPassDescriptors), std::move(_renderDestinations));
    return renderPassGraph;
}