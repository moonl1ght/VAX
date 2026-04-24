#pragma once

#include "luna.h"
#include "pipeline.h"
#include "device.h"

namespace vax::vk {
    class RenderPipeline final {
    public:
        explicit RenderPipeline(const vax::vk::Device& device) : _device(device) {};
    private:
        vax::utils::Logger _logger = vax::utils::Logger("RenderPipeline");
        std::reference_wrapper<const vax::vk::Device> _device;
    };
}