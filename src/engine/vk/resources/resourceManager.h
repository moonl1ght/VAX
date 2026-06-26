#pragma once

#include "bufferManager.h"
#include "luna.h"
#include "materialManager.h"
#include "meshManager.h"
#include "textureManager.h"
#include "ssboManager.h"

namespace vax {
class ResourceManager final {
  public:
    explicit ResourceManager(const vax::vk::Device& device, VmaAllocator allocator)
        : _bufferManager(device)
        , _meshManager(device)
        , _textureManager(device, allocator)
        , _materialManager(device)
        , _ssboManager(device) {};

    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) noexcept = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) noexcept = delete;

    bool setup(uint32_t maxInstances);

    void cleanup();

    vax::BufferManager& bufferManager() { return _bufferManager; }

    vax::MeshManager& meshManager() { return _meshManager; }

    vax::TextureManager& textureManager() { return _textureManager; }

    vax::MaterialManager& materialManager() { return _materialManager; }

    vax::SSBOManager& ssboManager() { return _ssboManager; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("ResourceManager");

    vax::BufferManager _bufferManager;
    vax::MeshManager _meshManager;
    vax::TextureManager _textureManager;
    vax::MaterialManager _materialManager;
    vax::SSBOManager _ssboManager;
};
} // namespace vax