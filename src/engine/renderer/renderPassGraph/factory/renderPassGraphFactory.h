#pragma once

#include "commandManager.h"
#include "descriptorSetManager.h"
#include "device.h"
#include "logger.h"
#include "pipelineManager.h"
#include "swapchain.h"
#include "queueManager.h"
#include "renderDestination.h"
#include "renderPassGraph.h"
#include "windowController.h"
#include "uiEngine.h"
#include <unordered_map>

namespace vax::engine {
class RenderPassGraphFactory final {
  public:
    RenderPassGraphFactory(
        VmaAllocator allocator,
        vax::vk::Device& device,
        vax::vk::PipelineManager& pipelineManager,
        vax::vk::DescriptorSetManager& descriptorSetManager,
        vax::WindowController& windowController,
        vax::ui::UIEngine& uiEngine
    )
        : _allocator(allocator)
        , _device(device)
        , _pipelineManager(pipelineManager)
        , _descriptorSetManager(descriptorSetManager)
        , _windowController(windowController)
        , _uiEngine(uiEngine) {};

    ~RenderPassGraphFactory() = default;

    void setupRenderPassDescriptors(VkFormat imageFormat);

    void setupRenderDestinations(
        vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
    );

    void setupRenderDestinationsForRoverCamera(
      vax::vk::CommandManager& commandManager, vax::vk::QueueManager& queueManager, vax::vk::Swapchain& swapchain
  );

    std::unique_ptr<RenderPassGraph> buildRoverDemoGraph();

    std::unique_ptr<RenderPassGraph> buildUiGraph();

  private:
    vax::Logger _logger = vax::Logger("rednerPassGraphFactory");
    VmaAllocator _allocator;
    std::reference_wrapper<vax::vk::Device> _device;
    std::reference_wrapper<vax::WindowController> _windowController;
    std::reference_wrapper<vax::vk::PipelineManager> _pipelineManager;
    std::reference_wrapper<vax::vk::DescriptorSetManager> _descriptorSetManager;
    std::reference_wrapper<vax::ui::UIEngine> _uiEngine;

    std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderPassDescriptor>> _renderPassDescriptors;
    std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderDestination>> _renderDestinations;
};
} // namespace vax::engine