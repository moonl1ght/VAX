#pragma once

#include "luna.h"
#include "bufferManager.h"

namespace vax {
    class ResourceManager final {
    public:
        ResourceManager(const vax::vk::Device& device) : _bufferManager(device) {};

        ResourceManager(const ResourceManager& other) = delete;
        ResourceManager(ResourceManager&& other) noexcept = delete;
        ResourceManager& operator=(const ResourceManager& other) = delete;
        ResourceManager& operator=(ResourceManager&& other) noexcept = delete;

        bool cleanup();

    private:
        vax::utils::Logger _logger = vax::utils::Logger("ResourceManager");

        vax::BufferManager _bufferManager;
    };
}