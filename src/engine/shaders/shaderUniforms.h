#ifndef shaderUniforms_h
#define shaderUniforms_h

#ifdef __cplusplus

#include <glm/glm.hpp>

using mat4 = glm::mat4;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

#else  // Slang

typedef float4x4 mat4;
typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

#endif

enum ObjectFlags {
    None = 0,
    IsWireframe = 1 << 0, // 0001
};

struct UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 cameraPosition;
};

typedef struct {
    mat4 model; // 64 bytes
    uint32_t flags; // 4 bytes
    uint32_t materialIndex; // 4 bytes
    uint32_t padding[2]; // 8 bytes
} DrawPushConstants;

typedef struct {
    vec4 baseColor; 
    float metallicFactor, roughnessFactor, normalScale, occlusionStrength; // vec4
    vec4 emissiveFactorAlphaCutoff;
} PBRMaterial;

#endif  // shaderUniforms_h