#include "renderPassGraphFactory.h"
#include "renderPass.h"

using namespace vax::engine;
using namespace vax::vk;

std::unique_ptr<RenderPassGraph> RenderPassGraphFactory::buildUiGraph() {
    auto swapchain = _windowController.get().getWindow(0)->getSwapchain();

    auto uiPass = std::make_shared<RenderPass>(
        "ui_pass",
        _device.get(),
        _pipelineManager.get(),
        _descriptorSetManager.get(),
        "UiPass",
        _renderDestinations.at("swapchain"),
        _renderPassDescriptors.at("swapchain")
    );
    uiPass->setRenderToSwapchain(true);
    uiPass->setRenderArea(VkRect2D{.offset = {0, 0}, .extent = swapchain->swapchainExtent});
    uiPass->setOutsideRenderPass(true);
    uiPass->setOutsideDrawWork([&uiEngine = _uiEngine.get()](RenderPass::RunPassInfo& runPassInfo) {
        uiEngine.render(runPassInfo.commandBuffer.vkCommandBuffer);
    });
    auto renderPassGraph =
        std::make_unique<RenderPassGraph>(uiPass, _renderPassDescriptors, _renderDestinations, _uiEngine.get());
    return renderPassGraph;
}
