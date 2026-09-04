#pragma once

#include "buffer.h"
#include "luna.h"
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

    void setVertices(const std::vector<VertexType>& vertices) { _vertices = vertices; }

    void addVertex(const VertexType& vertex) { _vertices.push_back(vertex); }

    void setIndices(const std::vector<uint32_t>& indices) { _indices = indices; }

    void addIndex(uint32_t index) { _indices.push_back(index); }

    void cleanupStagingBuffers();

    void setName(const std::string& name) { _name = name; }

  private:
    vax::Logger _logger = vax::Logger("Mesh");

    std::reference_wrapper<const Device> _device;
    std::reference_wrapper<const MeshManager> _meshManager;

    std::string _name;

    std::vector<VertexType> _vertices;
    std::vector<uint32_t> _indices;

    std::optional<Buffer<VertexType>> _stagingVertexBuffer = std::nullopt;
    std::optional<Buffer<uint32_t>> _stagingIndexBuffer = std::nullopt;

    bool _isLoaded = false;
    MeshId _id = NullId;

    void _destroy();
};

using Mesh = MeshObject<Vertex>;
using MeshPUV = MeshObject<VertexPUV>;
} // namespace vax::vk