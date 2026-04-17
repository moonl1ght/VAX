#ifndef Mesh_hpp
#define Mesh_hpp

#include "vkEngine.h"
#include "Vertex.h"
#include "buffer.h"
#include "luna.h"

class Mesh {
public:
    vax::vk::Buffer vertexBuffer;
    vax::vk::Buffer indexBuffer;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh(const vax::vk::Device& device) : vertexBuffer(device), indexBuffer(device) {};
    ~Mesh() {};

    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) noexcept = delete;

    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) noexcept = delete;

    void draw(vax::vk::Engine* vkEngine, VkCommandBuffer commandBuffer);
    void loadBuffers(vax::vk::Engine* vkEngine);

private:
    bool _isLoaded = false;
};

#endif