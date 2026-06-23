#ifndef shaderUniforms_h
#define shaderUniforms_h

#ifdef __cplusplus

#include <glm/ext/matrix_float3x4.hpp>
#include <glm/glm.hpp>

using mat4 = glm::mat4;
using mat3 = glm::mat3x4; // for cpu padding
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

#else // Slang

typedef float4x4 mat4;
typedef float3x3 mat3;
typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

#endif

enum SetIndices {
    GLOBAL_SET_INDEX = 0,
    PER_FRAME_SET_INDEX = 1,
    // PER_RENDER_PASS_SET_INDEX = 2,
    // PER_DRAW_SET_INDEX = 3,
};

enum FrameBindingIndices {
    FRAME_UNIFORM_BUFFER_INDEX = 0,
};

enum GlobalBindingIndices {
    GLOBAL_MATERIAL_BUFFER_INDEX = 0,
    GLOBAL_ENVIRONMENT_MAP_BUFFER_INDEX = 1,
    GLOBAL_SAMPLER_INDEX = 2,
    GLOBAL_TEXTURE_INDEX = 3,
};

enum VertexInputIndicesPUV {
    VERTEX_INPUT_PUV_POSITION_INDEX = 0,
    VERTEX_INPUT_PUV_UV_INDEX = 1,
};

enum VertexInputIndices {
    VERTEX_INPUT_POSITION_INDEX = 0,
    VERTEX_INPUT_PACKED_COLOR_INDEX = 1,
    VERTEX_INPUT_TANGENT_INDEX = 2,
    VERTEX_INPUT_NORMAL_INDEX = 3,
    VERTEX_INPUT_UV_INDEX = 4,
    VERTEX_INPUT_UV_2_INDEX = 5,
};

enum VertexInputIndicesNoTangent {
    VERTEX_INPUT_COLOR_INDEX_NT = 0,
    VERTEX_INPUT_POSITION_INDEX_NT = 1,
    VERTEX_INPUT_NORMAL_INDEX_NT = 2,
    VERTEX_INPUT_UV_INDEX_NT = 3,
    VERTEX_INPUT_UV_2_INDEX_NT = 4
};

enum { MAX_TEXTURES = 500, MAX_SAMPLERS = 4 };
static constexpr uint32_t NO_TEXTURE_FLAG = 0xFFFFFFFF;
static constexpr uint32_t NO_MATERIAL_INDEX = 0xFFFFFFFF;
static constexpr uint32_t NO_ENVIRONMENT_MAP_INDEX = 0xFFFFFFFF;
static constexpr uint32_t NO_SAMPLER_INDEX = 0xFFFFFFFF;

enum ObjectFlags {
    NoFlags = 0,
    IsWireframe = 1 << 0,    // 0001
    NoTangent = 1 << 1,      // 0010
    PrecomputedMVP = 1 << 2, // 0100
};

struct EnvironmentMapData {
    uint32_t envMapTexture;
    uint32_t envMapTextureSampler;

    uint32_t envMapTextureIrradiance;
    uint32_t envMapTextureIrradianceSampler;

    uint32_t texBRDFLUT;
    uint32_t texBRDFLUTSampler;

    uint32_t padding[2];
};

struct UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 cameraPosition;
    uint32_t environmentMapIndex = NO_ENVIRONMENT_MAP_INDEX;
    vec3 padding;
};

struct InstanceData {
    mat4 model;
    mat3 normalMatrix;
};

// TODO: now it uses normalMatrix that is for non uniform scaled objects if more data in the PushConstant is needed
// another Push constat layout is needed as well
struct DrawPushConstants {
    mat4 model;                                 // 64 bytes
    mat3 normalMatrix;                          // 48 (with padding) bytes
    uint32_t flags;                             // 4 bytes
    uint32_t materialIndex = NO_MATERIAL_INDEX; // 4 bytes
    uint32_t padding[2];                        // 8 bytes
};

struct PBRMaterial {
    vec4 baseColor;
    float metallicFactor, roughnessFactor, normalScale, occlusionStrength; // vec4
    vec4 emissiveFactorAlphaCutoff;

    uint32_t baseColorTextureIndex = NO_TEXTURE_FLAG;
    uint32_t baseColorTextureSamplerIndex = NO_SAMPLER_INDEX;
    uint32_t baseColorTextureUVIndex = 0;

    uint32_t normalMapTextureIndex = NO_TEXTURE_FLAG;
    uint32_t normalMapTextureSamplerIndex = NO_SAMPLER_INDEX;
    uint32_t normalMapTextureUVIndex = 0;

    uint32_t metallicRoughnessTextureIndex = NO_TEXTURE_FLAG;
    uint32_t metallicRoughnessTextureSamplerIndex = NO_SAMPLER_INDEX;
    uint32_t metallicRoughnessTextureUVIndex = 0;

    uint32_t aoTextureIndex = NO_TEXTURE_FLAG;
    uint32_t aoTextureSamplerIndex = NO_SAMPLER_INDEX;
    uint32_t aoTextureUVIndex = 0;

    uint32_t emissiveTextureIndex = NO_TEXTURE_FLAG;
    uint32_t emissiveTextureSamplerIndex = NO_SAMPLER_INDEX;
    uint32_t emissiveTextureUVIndex = 0;

    uint32_t alphaMode = 0;
};

#endif // shaderUniforms_h