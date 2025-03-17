//
// Created by Alexander Lakhonin on 12.02.2025.
//

#ifndef ShaderUtils_h
#define ShaderUtils_h

#include <metal_stdlib>

using namespace metal;

typedef struct {
  float3 position [[ attribute(kVertexAttributePosition) ]];
  float3 color [[ attribute(kVertexAttributeVertexColor) ]];
  float2 uv [[ attribute(kVertexAttributeUV) ]];
} VertexIn;

typedef struct {
  float4 position [[ position ]];
  float4 color;
  float2 uv;
} FragmentIn;

typedef FragmentIn VertexOut;

#endif /* ShaderUtils_h */
