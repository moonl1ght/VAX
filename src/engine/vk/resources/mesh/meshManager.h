#pragma once

#include "buffer.h"
#include "device.h"
#include "mesh.h"
#include "resourceHandle.h"
#include "vertex.h"

namespace vax::vk {
class MeshManager final {
  public:
    using VertexBuffer = Buffer<Vertex>;
    using VertexPUVBuffer = Buffer<VertexPUV>;
    using IndexBuffer = Buffer<uint32_t>;
    using MeshResource = std::pair<MeshHandle, Mesh*>;

    friend class MeshObject<vax::vk::Vertex>;

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

    std::optional<MeshResource> createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    std::optional<MeshResource> find(MeshHandle handle);

    bool deleteMesh(MeshHandle handle);

  private:
    struct ChunkInfo {
        size_t offset;
        size_t count;
    };

    struct BufferDescriptor {
        std::vector<ChunkInfo> memoryChunks;
        int maxNumberOfElements;
        int usedElements;
    };

    vax::Logger _logger = vax::Logger("MeshManager");

    std::reference_wrapper<const Device> _device;

    std::vector<std::unique_ptr<VertexBuffer>> _globalVertexBuffers;
    std::vector<std::unique_ptr<IndexBuffer>> _globalIndexBuffers;

    std::vector<BufferDescriptor> _vertexBufferDescriptors;
    std::vector<BufferDescriptor> _indexBufferDescriptors;

    std::unordered_map<MeshId, Mesh> _pool;
    MeshId _lastId = 0;
};
} // namespace vax::vk