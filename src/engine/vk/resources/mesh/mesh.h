#pragma once

#include "buffer.h"
#include "luna.h"
#include "resourceUtils.h"
#include "vertex.h"

namespace vax {
class MeshManager;
}

namespace vax::objects {
template <typename VertexType> class Mesh final {
  public:
    struct LoadMeshBuffersContext final {
        vax::vk::CommandBuffer* commandBuffer;
        uint32_t maxFramesInFlight;
    };

    friend class vax::MeshManager;

    std::optional<vax::vk::Buffer> vertexBuffer = std::nullopt;
    std::optional<vax::vk::Buffer> indexBuffer = std::nullopt;

    explicit Mesh(const vax::vk::Device& device, uint32_t instancesCount = 1)
        : _device(device) {};
    ~Mesh() { cleanup(); };

    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;

    Mesh(Mesh&& other) noexcept
        : _device(other._device)
        , _name(other._name)
        , _vertices(std::move(other._vertices))
        , _indices(std::move(other._indices))
        , _isLoaded(other._isLoaded)
        , _id(other._id)
        , _isDetached(other._isDetached)
        , vertexBuffer(std::move(other.vertexBuffer))
        , indexBuffer(std::move(other.indexBuffer)) {
        other._isLoaded = false;
        other._id = vax::NullId;
        other._isDetached = true;
        other._name.clear();
    }

    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            cleanup();
            _device = other._device;
            _name = std::move(other._name);
            _vertices = std::move(other._vertices);
            _indices = std::move(other._indices);
            vertexBuffer = std::move(other.vertexBuffer);
            indexBuffer = std::move(other.indexBuffer);
            _isLoaded = other._isLoaded;
            _id = other._id;
            _isDetached = other._isDetached;
        }
        return *this;
    }

    bool loadBuffers(const LoadMeshBuffersContext& context);

    bool isLoaded() const { return _isLoaded; }

    bool isDetached() const { return _isDetached; }

    MeshId id() const { return _id; }

    std::vector<VertexType>& vertices() { return _vertices; }

    std::vector<uint32_t>& indices() { return _indices; }

    void setVertices(const std::vector<VertexType>& vertices) { _vertices = vertices; }

    void addVertex(const VertexType& vertex) { _vertices.push_back(vertex); }

    void setIndices(const std::vector<uint32_t>& indices) { _indices = indices; }

    void addIndex(uint32_t index) { _indices.push_back(index); }

    void cleanup();

    void cleanupStagingBuffers();

    void setName(const std::string& name) { _name = name; }

  private:
    vax::Logger _logger = vax::Logger("Mesh");

    std::reference_wrapper<const vax::vk::Device> _device;
    std::string _name;

    std::vector<VertexType> _vertices;
    std::vector<uint32_t> _indices;

    std::optional<vax::vk::Buffer> _stagingVertexBuffer = std::nullopt;
    std::optional<vax::vk::Buffer> _stagingIndexBuffer = std::nullopt;

    bool _isLoaded = false;
    MeshId _id = vax::NullId;
    bool _isDetached = true;

    void _destroy();

    void _detach();
};

using MeshPBR = Mesh<Vertex>;
using MeshPUV = Mesh<VertexPUV>;
} // namespace vax::objects