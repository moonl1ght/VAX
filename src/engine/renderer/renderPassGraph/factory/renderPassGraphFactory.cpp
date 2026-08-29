#include "renderPassGraphFactory.h"
#include "renderDestinationBuilder.h"
#include "renderPassDescriptorBuilder.h"

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