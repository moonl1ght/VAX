import uniforms;

[vk::binding(FrameBindingIndices::FRAME_UNIFORM_BUFFER_INDEX, SetIndices::PER_FRAME_SET_INDEX)]
ConstantBuffer<UniformBufferObject> ubo;

[vk::binding(FrameBindingIndices::FRAME_INSTANCE_BUFFER_INDEX, SetIndices::PER_FRAME_SET_INDEX)]
StructuredBuffer<InstanceData> instances;

[vk::binding(GlobalBindingIndices::GLOBAL_MATERIAL_BUFFER_INDEX, SetIndices::GLOBAL_SET_INDEX)]
StructuredBuffer<PBRMaterial> materials;

[vk::binding(GlobalBindingIndices::GLOBAL_ENVIRONMENT_MAP_BUFFER_INDEX, SetIndices::GLOBAL_SET_INDEX)]
StructuredBuffer<EnvironmentMapData> environmentMaps;

[vk::binding(GlobalBindingIndices::GLOBAL_SAMPLER_INDEX, SetIndices::GLOBAL_SET_INDEX)]
SamplerState globalSamplers[MAX_SAMPLERS];

[vk::binding(GlobalBindingIndices::GLOBAL_TEXTURE_INDEX, SetIndices::GLOBAL_SET_INDEX)]
Texture2D globalTextures[MAX_TEXTURES];

[vk::binding(GlobalBindingIndices::GLOBAL_TEXTURE_INDEX, SetIndices::GLOBAL_SET_INDEX)]
TextureCube globalCubeTextures[MAX_TEXTURES];

struct VertexInput {
    [vk::location(VertexInputIndices::VERTEX_INPUT_POSITION_INDEX)]
    float3 position;

    [vk::location(VertexInputIndices::VERTEX_INPUT_PACKED_COLOR_INDEX)]
    uint packedColor;

    [vk::location(VertexInputIndices::VERTEX_INPUT_TANGENT_INDEX)]
    float4 tangent;

    [vk::location(VertexInputIndices::VERTEX_INPUT_NORMAL_INDEX)]
    float3 normal;

    [vk::location(VertexInputIndices::VERTEX_INPUT_UV_INDEX)]
    float2 uv;

    [vk::location(VertexInputIndices::VERTEX_INPUT_UV_2_INDEX)]
    float2 uv2;
};

struct VertexInputPUV {
    [vk::location(VertexInputIndicesPUV::VERTEX_INPUT_PUV_POSITION_INDEX)]
    float3 position;

    [vk::location(VertexInputIndicesPUV::VERTEX_INPUT_PUV_UV_INDEX)]
    float2 uv;
};

struct VertexOutput {
    float4 position : SV_Position;

    float4 tangent : TANGENT;

    float3 worldPosition : WORLD_POSITION;

    float3 normal : NORMAL;

    float2 uv : TEXCOORD0;

    float2 uv2 : TEXCOORD1;

    uint packedColor : PACKED_COLOR;
};

