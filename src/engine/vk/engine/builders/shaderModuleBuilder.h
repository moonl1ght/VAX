#pragma once

#include "luna.h"

namespace vax {
    class ShaderModuleBuilder final {
    public:
        ShaderModuleBuilder(const std::string& filename) {
            readFile(filename);
        }

        std::optional<VkShaderModule> build(VkDevice device) {
            if (_code.empty()) {
                _logger.error("Failed to build shader code!");
                return std::nullopt;
            }
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = _code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

            VkShaderModule shaderModule;
            if (!VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))) {
                _logger.error("Failed to build shader module!");
                return std::nullopt;
            }

            return shaderModule;
        }

    private:
        Logger _logger = Logger("ShaderModuleBuilder");
        std::vector<char> _code;

        void readFile(const std::string& filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                _logger.error("failed to open file!");
                _code = std::vector<char>();
                return;
            }

            size_t fileSize = (size_t)file.tellg();
            _code.resize(fileSize);

            file.seekg(0);
            file.read(_code.data(), fileSize);

            file.close();
        }
    };
}