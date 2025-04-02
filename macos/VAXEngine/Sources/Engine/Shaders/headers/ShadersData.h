//
// Created by Alexander Lakhonin on 02.04.2025.
//

#ifndef ShadersData_h
#define ShadersData_h

#include <simd/simd.h>

typedef struct ShadowVertexUniforms {
  matrix_float4x4 shadowProjectionMatrix;
  matrix_float4x4 shadowViewMatrix;

  inline int size() { return sizeof(matrix_float4x4) * 2; }

} ShadowVertexUniforms;

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
  vector_float3 cameraPosition;
  uint lightCount;
} FragmentUniforms;

typedef struct {
  int width;
  int height;
} TextureInfo;

typedef struct TextureIndices {
  int diffuseTextureIndex = -1;
  int specularTextureIndex = -1;
  int normalTextureIndex = -1;
  int heightTextureIndex = -1;
  int emissiveTextureIndex = -1;
  int roughnessTextureIndex = -1;
  int metallicTextureIndex = -1;
  int aoTextureIndex = -1;
} TextureIndices;

#endif /* ShadersData_h */
