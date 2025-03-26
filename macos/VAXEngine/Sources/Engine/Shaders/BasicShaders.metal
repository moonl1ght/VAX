//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include <metal_stdlib>
#include "ShaderTypes.h"
#include "ShaderUtils.h"
#include "Lighting.h"

using namespace metal;

[[host_name("basicVertex")]]
vertex VertexOut basic_vertex(
  const VertexIn in [[ stage_in ]],
  constant VertexUniforms &uniforms [[ buffer(kVertexUniformsBufferIndex) ]],
  constant ModelUniforms &modelUniforms [[ buffer(kModelUniformsBufferIndex) ]]
) {
  float4 position = uniforms.projectionMatrix * uniforms.viewMatrix * modelUniforms.modelMatrix * float4(in.position, 1.0);
  VertexOut out {
    .position = position,
    .normal = in.normal,
    .uv = in.uv,
    .color = float4(1.0, 0.0, 0.0, 1.0),
    .fragmentWorldPosition = modelUniforms.modelMatrix * float4(in.position, 1.0f),
    // we take only upper left of modelMatrix
    .worldNormal = float3(modelUniforms.modelMatrix * float4(in.normal, 0.0)),
    .worldTangent = float3(modelUniforms.modelMatrix * float4(in.tangent, 0.0)),
    .worldBitangent = float3(modelUniforms.modelMatrix * float4(in.bitangent, 0.0))
  };
  return out;
}

[[host_name("basicFragment")]]
fragment float4 basic_fragment(const FragmentIn in [[ stage_in ]]) {
  return in.color;
}

[[host_name("basicFragmentWithPhongLight")]]
fragment float4 basic_fragment_phong_lighting(
  const FragmentIn in [[ stage_in ]],
  constant FragmentUniforms &uniforms [[ buffer(3) ]],
  constant Light* lights [[ buffer(4) ]],
  constant TextureIndices& textureIndices [[ buffer(5) ]],
  constant TextureInfo* textureInfoBuffer [[ buffer(6) ]],
  texture2d_array<float> textureArray [[ texture(0) ]]
) {
  if (textureIndices.diffuseTextureIndex < 0 || textureIndices.diffuseTextureIndex >= int(textureArray.get_array_size())) {
    return float4(1.0, 0.0, 0.0, 1.0); // Return red color for invalid texture index
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

[[host_name("basicFragmentWithPBR")]]
fragment float4 basic_fragment_pbr(
  const FragmentIn in [[ stage_in ]],
  constant FragmentUniforms &uniforms [[ buffer(3) ]],
  constant Light* lights [[ buffer(4) ]],
  constant TextureIndices& textureIndices [[ buffer(5) ]],
  constant TextureInfo* textureInfoBuffer [[ buffer(6) ]],
  texture2d_array<float> textureArray [[ texture(0) ]]
) {
  constexpr sampler textureSampler(
    filter::linear,
    mip_filter::linear,
    max_anisotropy(8),
    address::repeat);
  if (!is_null_texture(textureArray)) {
    if (textureIndices.diffuseTextureIndex < 0 || textureIndices.diffuseTextureIndex >= int(textureArray.get_array_size())) {
      return float4(1.0, 0.0, 0.0, 1.0); // Return red color for invalid texture index
    }
    Material material;
    int idx = textureIndices.diffuseTextureIndex;
    float2 diffuseUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) /
                                float2(textureArray.get_width(0), textureArray.get_height(0)));
    const float3 diffuseColor = textureArray.sample(textureSampler, diffuseUV, idx).rgb;
    material.baseColor = diffuseColor;

    idx = textureIndices.metallicTextureIndex;
    if (idx > 0) {
      float2 metallicUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) /
                                   float2(textureArray.get_width(0), textureArray.get_height(0)));
      const float metallic = textureArray.sample(textureSampler, metallicUV, idx).r;
      material.metallic = metallic;
    } else {
      material.metallic = 0;
    }

    idx = textureIndices.roughnessTextureIndex;
    if (idx > 0) {
      float2 roughnessUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) /
                                    float2(textureArray.get_width(0), textureArray.get_height(0)));
      const float roughness = textureArray.sample(textureSampler, roughnessUV, idx).r;
      material.roughness = roughness;
    } else {
      material.roughness = 0;
    }

    idx = textureIndices.aoTextureIndex;
    if (idx > 0) {
      float2 ambientOcclusionUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) /
                                           float2(textureArray.get_width(0), textureArray.get_height(0)));
      const float ambientOcclusion = textureArray.sample(textureSampler, ambientOcclusionUV, idx).r;
      material.ambientOcclusion = ambientOcclusion;
    } else {
      material.ambientOcclusion = 1;
    }

    // normal map
    idx = textureIndices.normalTextureIndex;
    idx = idx == 0 ? textureIndices.heightTextureIndex : -1;
    float3 normal;
    if (idx > 0) {
      float2 normalUV = in.uv * (float2(textureInfoBuffer[idx].width, textureInfoBuffer[idx].height) /
                                 float2(textureArray.get_width(0), textureArray.get_height(0)));
      normal = textureArray.sample(textureSampler, normalUV, idx).rgb;
      normal = float3x3(in.worldTangent, in.worldBitangent, in.worldNormal) * normal;
      normal = normalize(normal);
    } else {
      normal = normalize(in.worldNormal);
    }

    float3 viewDirection = normalize(uniforms.cameraPosition);
    Light light = lights[0];
    float3 lightDirection = normalize(light.position);
    float3 F0 = mix(0.04, material.baseColor, material.metallic);

    float3 specularColor = computeSpecular(
      normal,
      viewDirection,
      lightDirection,
      material.roughness,
      F0);

    float3 finalDiffuseColor = computeDiffuse(
      material,
      normal,
      lightDirection);

//    float3 color = phongLighting(in.worldNormal,
//                                 in.fragmentWorldPosition.xyz,
//                                 uniforms,
//                                 lights,
//                                 diffuseColor);
    return float4(finalDiffuseColor + specularColor, 1.0f);
  } else {
    return float4(1.0, 0.0, 0.0, 1.0);
  }
}
