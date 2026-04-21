#pragma once
#include <array>
#include "luna.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace vax::objects {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 uv;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, uv);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && uv == other.uv;
        }
    };
}

namespace std {
    template<> struct hash<vax::objects::Vertex> {
        size_t operator()(vax::objects::Vertex const& vertex) const {
            return ((std::hash<glm::vec3>()(vertex.position) ^
                (std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (std::hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}