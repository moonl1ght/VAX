#pragma once

#include "buffer.h"
#include "resourceUtils.h"
#include "vertex.h"

namespace vax::vk {
class MeshManager;
}

namespace vax::vk {
template <typename VertexType> class MeshObject final {
  public:
    struct LoadMeshBuffersContext final {
        CommandBuffer* commandBuffer;
        uint32_t maxFramesInFlight;
    };

    friend class vax::vk::MeshManager;

    std::optional<Buffer<VertexType>> vertexBuffer = std::nullopt;
    std::optional<Buffer<uint32_t>> indexBuffer = std::nullopt;

    explicit MeshObject(const Device& device, const MeshManager& meshManager)
        : _device(device)
        , _meshManager(meshManager) {};

    ~MeshObject() {};

    MeshObject(const MeshObject& other) = delete;
    MeshObject& operator=(const MeshObject& other) = delete;

    MeshObject(MeshObject&& other) noexcept
        : _device(other._device)
        , _meshManager(other._meshManager)
        , _name(other._name)
        , _vertices(std::move(other._vertices))
        , _indices(std::move(other._indices))
        , _isLoaded(other._isLoaded)
        , _id(other._id)
        , vertexBuffer(std::move(other.vertexBuffer))
        , indexBuffer(std::move(other.indexBuffer)) {
        other._isLoaded = false;
        other._id = NullId;
        other._name.clear();
    }

    MeshObject& operator=(MeshObject&& other) noexcept {
        if (this != &other) {
            _destroy();
            _device = other._device;
            _meshManager = other._meshManager;
            _name = std::move(other._name);
            _vertices = std::move(other._vertices);
            _indices = std::move(other._indices);
            vertexBuffer = std::move(other.vertexBuffer);
            indexBuffer = std::move(other.indexBuffer);
            _isLoaded = other._isLoaded;
            _id = other._id;
        }
        return *this;
    }

    bool loadBuffers(const LoadMeshBuffersContext& context);

    bool isLoaded() const { return _isLoaded; }

    MeshId id() const { return _id; }

    std::vector<VertexType>& vertices() { return _vertices; }

    std::vector<uint32_t>& indices() { return _indices; }

    void setVertices(std::vector<VertexType> vertices);

    void addVertex(const VertexType& vertex);

    void setIndices(std::vector<uint32_t> indices);

    void addIndex(uint32_t index);

    void cleanupStagingBuffers();

    void setName(const std::string& name) { _name = name; }

    void bindToCommandBuffer(CommandBuffer* commandBuffer);

    bool bindBuffers();

    void lock();

    void unlock();

    bool isLocked() const { return _locked; }

    bool isBound() const { return _bound; }

  private:
    vax::Logger _logger = vax::Logger("Mesh");

    std::reference_wrapper<const Device> _device;
    std::reference_wrapper<const MeshManager> _meshManager;

    std::string _name;

    MeshId _id = NullId;

    std::vector<VertexType> _vertices;
    std::vector<uint32_t> _indices;

    bool _bound = false;
    bool _locked = false; // if true, the mesh is locked and cannot be modified

    std::optional<Buffer<VertexType>> _stagingVertexBuffer = std::nullopt;
    std::optional<Buffer<uint32_t>> _stagingIndexBuffer = std::nullopt;

    bool _isLoaded = false;

    void _destroy();
};

using Mesh = MeshObject<Vertex>;
using MeshPUV = MeshObject<VertexPUV>;
} // namespace vax::vk