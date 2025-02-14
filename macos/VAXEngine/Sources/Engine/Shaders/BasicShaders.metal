//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include <metal_stdlib>
#include "ShaderTypes.h"
#include "ShaderUtils.h"

using namespace metal;

[[host_name("basicVertex")]]
vertex VertexOut basic_vertex(const VertexIn in [[ stage_in ]]) {
//  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * float4(in.position, 1.0);
  VertexOut out {
    .position = float4(in.position, 1.0),
//    .color = float4(in.color, 1.0)
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentIn in [[ stage_in ]]) {
  return float4(1, 0, 0, 1.0);
}
