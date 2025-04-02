//
// Created by Alexander Lakhonin on 26.03.2025.
//

#include <metal_stdlib>
#include "headers/ShadersData.h"
#include "headers/AttributesAndIndices.h"

using namespace metal;

struct DVertexIn {
  float3 position [[ attribute(kVertexAttributePosition) ]];
  float3 color [[ attribute(kVertexAttributeVertexColor) ]];
};

struct DVertexOut {
  float4 position [[ position ]];
  float point_size [[ point_size ]];
  float4 color;
};

[[host_name("debugVertex")]]
vertex DVertexOut debug_vertex(
  const DVertexIn in [[ stage_in ]],
  constant VertexUniforms &uniforms [[ buffer(kVertexUniformsBufferIndex) ]],
  constant ModelUniforms &modelUniforms [[ buffer(kModelUniformsBufferIndex) ]])
{
  matrix_float4x4 mvp = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix;
  DVertexOut out {
    .position = mvp * float4(in.position, 1),
    .point_size = 25.0,
    .color = float4(in.color, 1)
  };
  return out;
}

[[host_name("debugFragment")]]
fragment float4 debug_fragment(const DVertexOut in [[ stage_in ]]) {
  return in.color;
}
