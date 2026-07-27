#pragma once

#include "commandBuffer.h"
#include "logger.h"
#include "renderDestination.h"
#include "swapchain.h"
#include "uiEngine.h"
#include "vkEngine.h"

namespace vax::engine {
class BaseRenderer {
  public:
    BaseRenderer(vax::vk::Engine& vkEngine, vax::ui::UIEngine& uiEngine)
        : _vkEngine(vkEngine)
        , _uiEngine(uiEngine) {};

    virtual ~BaseRenderer() {};

    BaseRenderer(const BaseRenderer& other) = delete;
    BaseRenderer& operator=(const BaseRenderer& other) = delete;
    BaseRenderer(BaseRenderer&& other) noexcept = delete;
    BaseRenderer& operator=(BaseRenderer&& other) noexcept = delete;

    void setViewportAndScissor(vax::vk::CommandBuffer& commandBuffer, VkExtent2D extent);

  protected:
    vax::Logger _logger = vax::Logger("BaseRenderer");

    std::reference_wrapper<vax::vk::Engine> _vkEngine;
    std::reference_wrapper<vax::ui::UIEngine> _uiEngine;

    std::unordered_map<std::string, vax::vk::RenderPassDescriptor> _renderPassDescriptors;
    std::unordered_map<std::string, vax::vk::RenderDestination> _renderDestinations;

    uint32_t _currentFrame = 0;

    vax::vk::Swapchain* _getSwapchain() {
        return _vkEngine.get().getWindowController().getPrimaryWindow()->getSwapchain();
    };
};
} // namespace vax::engine