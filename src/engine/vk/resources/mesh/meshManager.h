#pragma once

#include "device.h"
#include "buffer.h"
#include "mesh.h"
#include "resourceHandle.h"

namespace vax::vk {
class MeshManager final {
  public:
    using MeshResource = std::pair<MeshHandle, MeshPBR*>;

    explicit MeshManager(const Device& device)
        : _device(device) {};

    ~MeshManager() { fullCleanup(); }

    MeshManager(const MeshManager& other) = delete;
    MeshManager(MeshManager&& other) noexcept = delete;
    MeshManager& operator=(const MeshManager& other) = delete;
    MeshManager& operator=(MeshManager&& other) noexcept = delete;

    void fullCleanup();

    std::optional<MeshResource> createEmptyMesh(uint32_t instancesCount = 1);

    std::optional<MeshResource> find(MeshHandle handle);

    bool deleteMesh(MeshHandle handle);

    std::optional<MeshPBR> detach(MeshHandle handle);

  private:
    vax::Logger _logger = vax::Logger("MeshManager");

    std::reference_wrapper<const Device> _device;
    std::unique_ptr<Buffer> _globalVertexBuffer;
    std::unique_ptr<Buffer> _globalIndexBuffer;
    // TODO: change to vector + use generation for stability
    // maybe vector of vectors of buffers?
    std::unordered_map<MeshId, MeshPBR> _pool;
    MeshId _lastId = 0;
};
} // namespace vax