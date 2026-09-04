#pragma once

#include "buffer.h"
#include "device.h"
#include "mesh.h"
#include "resourceHandle.h"

namespace vax::vk {
class MeshManager final {
  public:
    using VertexBuffer = Buffer<Vertex>;
    using VertexPUVBuffer = Buffer<VertexPUV>;
    using IndexBuffer = Buffer<uint32_t>;
    using MeshResource = std::pair<MeshHandle, Mesh*>;

    explicit MeshManager(const Device& device)
        : _device(device) {};

    ~MeshManager() { fullCleanup(); }

    MeshManager(const MeshManager& other) = delete;
    MeshManager(MeshManager&& other) noexcept = delete;
    MeshManager& operator=(const MeshManager& other) = delete;
    MeshManager& operator=(MeshManager&& other) noexcept = delete;

    /// size in MB
    void loadGlobalBuffers(size_t size);

    void fullCleanup();

    std::optional<MeshResource> createEmptyMesh();

    std::optional<MeshResource> find(MeshHandle handle);

    bool deleteMesh(MeshHandle handle);

  private:
    vax::Logger _logger = vax::Logger("MeshManager");

    std::reference_wrapper<const Device> _device;

    std::vector<std::unique_ptr<VertexBuffer>> _globalVertexBuffers;
    std::vector<std::unique_ptr<IndexBuffer>> _globalIndexBuffers;

    // TODO: change to vector + use generation for stability2
    // maybe vector of vectors of buffers?
    std::unordered_map<MeshId, Mesh> _pool;
    MeshId _lastId = 0;
};
} // namespace vax::vk