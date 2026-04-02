#ifndef Mesh_hpp
#define Mesh_hpp

#include "vkEngine.h"
#include "Vertex.h"
#include "Buffer.hpp"
#include "luna.h"

class Mesh {
public:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh() {};
    ~Mesh() {};

    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) noexcept {
        std::swap(vertexBuffer, other.vertexBuffer);
        std::swap(indexBuffer, other.indexBuffer);
        std::swap(vertices, other.vertices);
        std::swap(indices, other.indices);
    }

    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            vertexBuffer = std::move(other.vertexBuffer);
            indexBuffer = std::move(other.indexBuffer);
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
        }
        return *this;
    }

    void draw(vax::VkEngine* vkEngine, VkCommandBuffer commandBuffer);
    void loadBuffers(vax::VkEngine* vkEngine);

private:
    bool _isLoaded = false;
};

#endif