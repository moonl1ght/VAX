#include "rednerPassGraphFactory.h"
#include "renderPassDescriptorBuilder.h"

using namespace vax::engine;
using namespace vax::vk;

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