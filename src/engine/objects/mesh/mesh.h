#pragma once

#include "vkEngine.h"
#include "vertex.h"
#include "buffer.h"
#include "luna.h"

namespace vax::objects {
    class Mesh final {
    public:
        std::optional<vax::vk::Buffer> vertexBuffer = std::nullopt;
        std::optional<vax::vk::Buffer> indexBuffer = std::nullopt;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        explicit Mesh(const vax::vk::Device& device) : vertexBuffer(device) {};
        ~Mesh() {};

        Mesh(const Mesh& other) = delete;
        Mesh(Mesh&& other) noexcept = delete;
        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) noexcept = delete;

        bool draw(VkCommandBuffer commandBuffer) const;

        void forceDraw(vax::vk::Engine* vkEngine, VkCommandBuffer commandBuffer);

        bool loadBuffers(vax::vk::Engine* vkEngine);

    private:
        utils::Logger _logger = utils::Logger("Mesh");
        bool _isLoaded = false;
    };
}