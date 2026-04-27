#include "shaderUniforms.h"

[vk::binding(0, 0)]
ConstantBuffer<UniformBufferObject> ubo;

// [vk::binding(1, 0)]
// Sampler2D texSampler;

[vk::push_constant]
DrawPushConstants pushConstants;

struct VertexInput {
    [vk::location(0)]
    float3 position;

    [vk::location(1)]
    float3 color;

    [vk::location(2)]
    float2 uv;
};

struct VertexOutput {
    float4 position : SV_Position;

    [vk::location(0)]
    float3 color;

    [vk::location(1)]
    float2 uv;
};
