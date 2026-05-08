#include "shaderUniforms.h"

[vk::binding(FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX, SetIndices::PER_FRAME_SET_INDEX)]
ConstantBuffer<UniformBufferObject> ubo;

[vk::binding(GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX, SetIndices::GLOBAL_SET_INDEX)]
StructuredBuffer<PBRMaterial> materials;

[vk::binding(GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, SetIndices::GLOBAL_SET_INDEX)]
SamplerState globalSamplers[MAX_SAMPLERS];

[vk::binding(GlobalBindingIndices::GLOBAL_TEXTURE_INDEX, SetIndices::GLOBAL_SET_INDEX)]
Texture2D globalTextures[MAX_TEXTURES];

[vk::push_constant]
DrawPushConstants pushConstants;

struct VertexInput {
    [vk::location(VertexInputIndices::VERTEX_INPUT_POSITION_INDEX)]
    float3 position;

    [vk::location(VertexInputIndices::VERTEX_INPUT_NORMAL_INDEX)]
    float3 normal;

    [vk::location(VertexInputIndices::VERTEX_INPUT_TANGENT_INDEX)]
    float3 tangent;

    [vk::location(VertexInputIndices::VERTEX_INPUT_UV_INDEX)]
    float2 uv;
};

struct VertexOutput {
    float4 position : SV_Position;

    float3 normal : NORMAL;

    float2 uv : TEXCOORD0;
};

