//
// Created by Alexander Lakhonin on 12.02.2025.
//

#ifndef ShaderUtils_h
#define ShaderUtils_h

#include <metal_stdlib>

using namespace metal;

typedef struct {
  float3 position [[ attribute(kVertexAttributePosition) ]];
  float3 normal [[ attribute(kVertexAttributeNormal) ]];
  float3 color [[ attribute(kVertexAttributeVertexColor) ]];
  float2 uv [[ attribute(kVertexAttributeUV) ]];
  float2 tangent [[ attribute(kVertexAttributeTangent) ]];
  float2 bitangent [[ attribute(kVertexAttributeBitangent) ]];
} VertexIn;

typedef struct {
  float4 position [[ position ]];
  float4 color;
  float4 fragmentWorldPosition;
  float3 worldNormal;
  float3 normal;
  float3 worldTangent;
  float3 worldBitangent;
  float2 uv;
} FragmentIn;

typedef FragmentIn VertexOut;

#endif /* ShaderUtils_h */
