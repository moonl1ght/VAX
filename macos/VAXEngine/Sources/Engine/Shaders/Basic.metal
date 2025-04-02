//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include <metal_stdlib>
#include "headers/AttributesAndIndices.h"
#include "headers/VertexData.h"
#include "Lighting.h"

using namespace metal;

[[host_name("primitiveVertex")]]
vertex VertexOutPrimitive primitive_vertex(
  const VertexInPrimitive in [[ stage_in ]],
                              constant VertexUniforms &uniforms [[ buffer(kVertexUniformsBufferIndex) ]],
                              constant ModelUniforms &modelUniforms [[ buffer(kModelUniformsBufferIndex) ]]
                              )
{
  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix * float4(in.position, 1.0);
  VertexOutPrimitive out {
    .position = position,
    .color = float4(in.color, 1.0),
  };
  return out;
}

[[host_name("basicVertex")]]
vertex VertexOut basic_vertex(
  const VertexIn in [[ stage_in ]],
  constant VertexUniforms &uniforms [[ buffer(kVertexUniformsBufferIndex) ]],
  constant ModelUniforms &modelUniforms [[ buffer(kModelUniformsBufferIndex) ]],
  constant ShadowVertexUniforms &shadowUniforms [[ buffer(kShadowUniformsBufferIndex) ]]
) {
  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix * float4(in.position, 1.0);
  float4 shadowPosition = shadowUniforms.shadowProjectionMatrix * shadowUniforms.shadowViewMatrix *
  modelUniforms.modelMatrix * float4(in.position, 1.0);
  VertexOut out {
    .position = position,
    .normal = in.normal,
    .uv = in.uv,
    .color = float4(in.color, 1.0f),
    .fragmentWorldPosition = modelUniforms.modelMatrix * float4(in.position, 1.0f),
    // we take only upper left of modelMatrix
    .worldNormal = float3(modelUniforms.modelMatrix * float4(in.normal, 0.0)),
    .worldTangent = float3(modelUniforms.modelMatrix * float4(in.tangent, 0.0)),
    .worldBitangent = float3(modelUniforms.modelMatrix * float4(in.bitangent, 0.0)),
    .shadowPosition = shadowPosition
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentInPrimitive in [[ stage_in ]]) {
  return in.color;
}

[[host_name("basicFragmentWithPhongLight")]]
fragment float4 basic_fragment_phong_lighting(
  const FragmentIn in [[ stage_in ]],
  constant FragmentUniforms &uniforms [[ buffer(kFragmentUniformsIndex) ]],
  constant Light* lights [[ buffer(kLightIndex) ]],
  constant TextureIndices& textureIndices [[ buffer(kTextureIndicesIndex) ]],
  constant TextureInfo* textureInfoBuffer [[ buffer(kTextureInfoIndex) ]],
  texture2d_array<float> textureArray [[ texture(kTextureArrayIndex) ]],
  depth2d<float> shadowTexture [[ texture(kTextureShadowIndex) ]]
) {
  if (textureIndices.diffuseTextureIndex < 0 || textureIndices.diffuseTextureIndex >= int(textureArray.get_array_size())) {
    return in.color;//float4(1.0, 0.0, 0.0, 1.0); // Return red color for invalid texture index
  }
  constexpr sampler textureSampler(filter::linear,
                                   mip_filter::linear,
                                   max_anisotropy(8),
                                   address::repeat);
  if (!is_null_texture(textureArray)) {
    int idx = textureIndices.diffuseTextureIndex;
    float2 transformedDiffuseUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) / float2(textureArray.get_width(0), textureArray.get_height(0)));
    const float3 diffuseColor = textureArray.sample(textureSampler,
                                                     transformedDiffuseUV,
                                                     textureIndices.diffuseTextureIndex).rgb;
//    float3 baseColor = baseColorTexture.sample(textureSampler,
//                                               in.uv * params.tiling).rgb;
    float3 color = phongLighting(normalize(in.worldNormal),
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
