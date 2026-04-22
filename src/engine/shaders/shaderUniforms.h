#ifndef ShaderUniforms_h
#define ShaderUniforms_h

#ifdef __cplusplus

#include <glm/glm.hpp>

using mat4 = glm::mat4;
using vec2 = glm::vec2;
using vec3 = glm::vec3;

#else  // Slang

typedef float4x4 mat4;
typedef float2 vec2;
typedef float3 vec3;

#endif

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

struct DrawPushConstants {
    mat4 model; // 64 bytes
};

#endif  // ShaderUniforms_h