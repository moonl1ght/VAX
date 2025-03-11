//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include <metal_stdlib>
#include "ShaderTypes.h"
#include "ShaderUtils.h"

using namespace metal;

[[host_name("basicVertex")]]
vertex VertexOut basic_vertex(const VertexIn in [[ stage_in ]],
                              constant VertexUniforms &uniforms [[ buffer(10) ]],
                              constant ModelUniforms &modelUniforms [[ buffer(11) ]]) {
  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix * float4(in.position, 1.0);
  VertexOut out {
    .position = position,
    .color = float4(in.color, 1.0)
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentIn in [[ stage_in ]]) {
  return in.color;
}

[[host_name("basicFragmentWithPhongLight")]]
fragment float4 basic_fragment_phong_lighting(const FragmentIn in [[ stage_in ]],
                                              constant FragmentUniforms &uniforms [[ buffer(1) ]],
                                              constant Light* light [[ buffer(2) ]]) {
  return in.color;
}
