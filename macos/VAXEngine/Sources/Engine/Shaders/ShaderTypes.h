//
// Created by Alexander Lakhonin on 10.02.2025.
//

#ifndef ShaderTypes_h
#define ShaderTypes_h

#include <simd/simd.h>

typedef enum {
  unused = 0,
  Sunlight = 1,
  Spotlight = 2,
  Pointlight = 3,
  Ambientlight = 4
} LightType;

typedef struct {
  vector_float3 baseColor;
  vector_float3 specularColor;
  float roughness;
  float metallic;
  float ambientOcclusion;
  float shininess;
} Material;

typedef struct {
  vector_float3 position;
  vector_float3 color;
  vector_float3 specularColor;
  float intensity;
  vector_float3 attenuation;
  LightType type;
  float coneAngle;
  vector_float3 coneDirection;
  float coneAttenuation;
} Light;

typedef enum VertexAttributes {
  kVertexAttributePosition = 0,
  kVertexAttributeNormal = 1,
  kVertexAttributeVertexColor = 2,
  kVertexAttributeUV = 3,
  kVertexAttributeTangent = 4,
  kVertexAttributeBitangent = 5
} VertexAttributes;

typedef enum BufferIndex {
  kBufferIndexMeshPositions = 0,
} BufferIndex;

typedef enum {
  kVertexBufferIndex = 0,
  kUVBufferIndex = 1,
  kVertexUniformsBufferIndex = 10,
  kModelUniformsBufferIndex = 11
} VertexBufferIndices;

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

#endif /* ShaderTypes_h */
