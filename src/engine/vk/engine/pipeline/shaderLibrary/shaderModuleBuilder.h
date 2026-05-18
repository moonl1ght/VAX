#pragma once

#include "luna.h"
#include "device.h"

namespace vax::vk {
    class ShaderModuleBuilder final {
    public:
        ShaderModuleBuilder(const vax::vk::Device& device) : _device(device) {
        }

        std::optional<VkShaderModule> build(const std::string& filename);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("ShaderModuleBuilder");
        std::reference_wrapper<const vax::vk::Device> _device;

        std::vector<char> _readFile(const std::string& filename) const;
    };
}