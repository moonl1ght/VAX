#pragma once

#include "vertex.h"
#include "buffer.h"
#include "luna.h"
#include "resourceUtils.h"

namespace vax {
    class MeshManager;
}

namespace vax::objects {
    class Mesh final {
    public:
        friend class vax::MeshManager;

        std::optional<vax::vk::Buffer> vertexBuffer = std::nullopt;
        std::optional<vax::vk::Buffer> indexBuffer = std::nullopt;

        explicit Mesh(const vax::vk::Device& device) : _device(device) {};
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

        bool loadBuffers(
            vax::vk::CommandBuffer& commandBuffer
        );

        bool isLoaded() const { return _isLoaded; }

        bool isDetached() const { return _isDetached; }

        MeshId id() const { return _id; }

        std::vector<Vertex>& vertices() { return _vertices; }

        std::vector<uint32_t>& indices() { return _indices; }

        void setVertices(const std::vector<Vertex>& vertices) { _vertices = vertices; }

        void addVertex(const Vertex& vertex) { _vertices.push_back(vertex); }

        void setIndices(const std::vector<uint32_t>& indices) { _indices = indices; }

        void addIndex(uint32_t index) { _indices.push_back(index); }

        void cleanup();

        void cleanupStagingBuffers();

        void setName(const std::string& name) { _name = name; }

    private:
        utils::Logger _logger = utils::Logger("Mesh");

        std::reference_wrapper<const vax::vk::Device> _device;
        std::string _name;

        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;

        std::optional<vax::vk::Buffer> _stagingVertexBuffer = std::nullopt;
        std::optional<vax::vk::Buffer> _stagingIndexBuffer = std::nullopt;

        bool _isLoaded = false;
        MeshId _id = vax::NullId;
        bool _isDetached = true;

        void _destroy();

        void _detach();
    };
}