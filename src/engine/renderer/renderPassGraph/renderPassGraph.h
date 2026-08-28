#pragma once

#include <unordered_map>

#include "renderDestination.h"
#include "renderPassDescriptor.h"

namespace vax::engine {

class RenderPassGraph {
  public:
    RenderPassGraph(
        std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderPassDescriptor>> renderPassDescriptors,
        std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderDestination>> renderDestinations
    )
        : _renderPassDescriptors(std::move(renderPassDescriptors))
        , _renderDestinations(std::move(renderDestinations)) {};

    RenderPassGraph(const RenderPassGraph&) = delete;
    RenderPassGraph(RenderPassGraph&&) = default;
    RenderPassGraph& operator=(const RenderPassGraph&) = delete;
    RenderPassGraph& operator=(RenderPassGraph&&) = default;

    ~RenderPassGraph() = default;

    void run();

  private:
    std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderPassDescriptor>> _renderPassDescriptors;
    std::unordered_map<std::string, std::shared_ptr<vax::vk::RenderDestination>> _renderDestinations;
};

} // namespace vax::engine