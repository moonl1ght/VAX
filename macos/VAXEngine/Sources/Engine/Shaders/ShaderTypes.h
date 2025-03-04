//
// Created by Alexander Lakhonin on 10.02.2025.
//

#ifndef ShaderTypes_h
#define ShaderTypes_h

#include <simd/simd.h>

typedef enum VertexAttributes {
  kVertexAttributePosition = 0,
  kVertexAttributeNormal = 1,
  kVertexAttributeUV = 2,
  kVertexAttributeVertexColor = 3,
  kVertexAttributeTangent = 4,
  kVertexAttributeBitangent = 5
} VertexAttributes;

typedef enum BufferIndex {
  kBufferIndexMeshPositions = 0,
} BufferIndex;

typedef struct VertexUniforms {
  matrix_float4x4 viewMatrix;
  matrix_float4x4 projectionMatrix;

  inline int size() { return sizeof(matrix_float4x4) * 2; }

} VertexUniforms;

typedef struct ModelUniforms {
  matrix_float4x4 modelMatrix;

  inline int size() { return sizeof(matrix_float4x4); }
} ModelUniforms;

typedef struct {
} FragmentUniforms;

#endif /* ShaderTypes_h */
