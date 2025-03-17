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
    .uv = in.uv,
    .color = float4(1.0, 0.0, 0.0, 1.0)
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentIn in [[ stage_in ]]) {
  return in.color;
}

[[host_name("basicFragmentWithPhongLight")]]
fragment float4 basic_fragment_phong_lighting(const FragmentIn in [[ stage_in ]],
//                                              constant FragmentUniforms &uniforms [[ buffer(1) ]],
//                                              constant Light* light [[ buffer(2) ]],
                                              constant TextureIndices& textureIndices [[ buffer(3) ]],
                                              texture2d_array<float> textureArray [[ texture(0) ]]) {
  constexpr sampler textureSampler(filter::linear,
                                   mip_filter::linear,
                                   max_anisotropy(8),
                                   address::repeat);
  if (!is_null_texture(textureArray)) {
    float2 transformedDiffuseUV = in.uv * (float2(2048, 2048) / float2(textureArray.get_width(0), textureArray.get_height(0)));
    const float3 diffuseSample = textureArray.sample(textureSampler, transformedDiffuseUV, textureIndices.diffuseTextureIndex).rgb;
//    float3 baseColor = baseColorTexture.sample(textureSampler,
//                                               in.uv * params.tiling).rgb;
    return float4(diffuseSample, 1.0f);
  } else {
    return float4(0.0, 1.0, 0.0, 1.0);
  }
//  return in.color;
}
