#ifndef VertexUniforms_h
#define VertexUniforms_h

#ifdef __cplusplus

#include <glm/glm.hpp>

using mat4 = glm::mat4;
using vec2 = glm::vec2;
using vec3 = glm::vec3;

#endif  // __cplusplus

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

#endif  // VertexUniforms_h