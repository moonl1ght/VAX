#pragma once
#include <array>
#include "luna.h"
#include "shaderUniforms.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace vax::objects {
    struct VertexNoTangent {
        glm::vec4 color;
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec2 uv2;
        float padding[2];
    };

    struct Vertex {
        static constexpr Vertex empty() {
            return Vertex{
                glm::vec3(0.0f, 0.0f, 0.0f), 0,
                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                0
            };
        };

        glm::vec3 position;
        uint32_t packedColor;
        glm::vec4 tangent;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec2 uv2;
        uint32_t padding;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = VertexInputIndices::VERTEX_INPUT_POSITION_INDEX;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = VertexInputIndices::VERTEX_INPUT_PACKED_COLOR_INDEX;
            attributeDescriptions[1].format = VK_FORMAT_R32_UINT;
            attributeDescriptions[1].offset = offsetof(Vertex, packedColor);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = VertexInputIndices::VERTEX_INPUT_TANGENT_INDEX;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, tangent);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = VertexInputIndices::VERTEX_INPUT_NORMAL_INDEX;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, normal);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = VertexInputIndices::VERTEX_INPUT_UV_INDEX;
            attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(Vertex, uv);

            attributeDescriptions[5].binding = 0;
            attributeDescriptions[5].location = VertexInputIndices::VERTEX_INPUT_UV_2_INDEX;
            attributeDescriptions[5].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[5].offset = offsetof(Vertex, uv2);

            return attributeDescriptions;
        }
    };
}