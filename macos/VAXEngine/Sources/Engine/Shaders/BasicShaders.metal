//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include <metal_stdlib>
#include "ShaderTypes.h"
#include "ShaderUtils.h"
#include "Lighting.h"

using namespace metal;

[[host_name("basicVertex")]]
vertex VertexOut basic_vertex(const VertexIn in [[ stage_in ]],
                              constant VertexUniforms &uniforms [[ buffer(10) ]],
                              constant ModelUniforms &modelUniforms [[ buffer(11) ]]) {
  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix * float4(in.position, 1.0);
  VertexOut out {
    .position = position,
    .normal = in.normal,
    .uv = in.uv,
    .color = float4(1.0, 0.0, 0.0, 1.0),
    .fragmentWorldPosition = modelUniforms.modelMatrix * float4(in.position, 1.0f),
    // we take only upper left of modelMatrix
    .worldNormal = normalize(float3(modelUniforms.modelMatrix * float4(in.normal, 0.0)))
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentIn in [[ stage_in ]]) {
  return in.color;
}

[[host_name("basicFragmentWithPhongLight")]]
fragment float4 basic_fragment_phong_lighting(const FragmentIn in [[ stage_in ]],
                                              constant FragmentUniforms &uniforms [[ buffer(3) ]],
                                              constant Light* lights [[ buffer(4) ]],
                                              constant TextureIndices& textureIndices [[ buffer(5) ]],
                                              texture2d_array<float> textureArray [[ texture(0) ]]) {
  constexpr sampler textureSampler(filter::linear,
                                   mip_filter::linear,
                                   max_anisotropy(8),
                                   address::repeat);
  if (!is_null_texture(textureArray)) {
    float2 transformedDiffuseUV = in.uv * (float2(2048, 2048) / float2(textureArray.get_width(0), textureArray.get_height(0)));
    const float3 diffuseColor = textureArray.sample(textureSampler,
                                                     transformedDiffuseUV,
                                                     textureIndices.diffuseTextureIndex).rgb;
//    float3 baseColor = baseColorTexture.sample(textureSampler,
//                                               in.uv * params.tiling).rgb;
    float3 color = phongLighting(in.worldNormal,
                                 in.fragmentWorldPosition.xyz,
                                 uniforms,
                                 lights,
                                 diffuseColor);
    return float4(color, 1.0f);
  } else {
    return float4(0.0, 1.0, 0.0, 1.0);
  }
//  return in.color;
}
