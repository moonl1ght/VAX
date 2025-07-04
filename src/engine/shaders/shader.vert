#version 460

#extension GL_GOOGLE_include_directive : enable

#include "VertexUniforms.h"

layout(binding = 0) uniform _UniformBufferObject {
    UniformBufferObject ubo;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;

void main() {
    UniformBufferObject ubo = ubo.ubo;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUv = inUv;
}