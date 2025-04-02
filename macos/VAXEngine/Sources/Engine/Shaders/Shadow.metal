//
// Created by Alexander Lakhonin on 02.04.2025.
//

#include <metal_stdlib>
#include "headers/AttributesAndIndices.h"
#include "headers/ShadersData.h"

using namespace metal;

struct VertexIn {
  float4 position [[ attribute(kVertexAttributePosition) ]];
};

vertex float4 depthVertex(
  const VertexIn in [[stage_in]],
  constant ShadowVertexUniforms &uniforms [[ buffer(kVertexUniformsBufferIndex) ]],
  constant ModelUniforms &modelUniforms [[ buffer(kModelUniformsBufferIndex) ]])
{
  matrix_float4x4 mvp = uniforms.shadowProjectionMatrix * uniforms.shadowViewMatrix * modelUniforms.modelMatrix;
  return mvp * in.position;
}
