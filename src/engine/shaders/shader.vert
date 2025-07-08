#version 460

#extension GL_GOOGLE_include_directive : enable

#include "ShaderUniforms.h"

layout(set = 0, binding = 0) uniform _UniformBufferObject {
    UniformBufferObject ubo;
} ubo;

layout(push_constant) uniform _DrawPushConstants {
    DrawPushConstants drawPushConstants;
} drawPushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;

void main() {
    DrawPushConstants drawPushConstants = drawPushConstants.drawPushConstants;
    UniformBufferObject ubo = ubo.ubo;
    gl_Position = ubo.proj * ubo.view * drawPushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUv = inUv;
}