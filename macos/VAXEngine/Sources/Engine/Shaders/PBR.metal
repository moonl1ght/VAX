//
// Created by Alexander Lakhonin on 02.04.2025.
//

#include <metal_stdlib>
#include "headers/AttributesAndIndices.h"
#include "headers/VertexData.h"
#include "Lighting.h"

using namespace metal;

[[host_name("basicFragmentWithPBR")]]
fragment float4 basic_fragment_pbr(
  const FragmentIn in [[ stage_in ]],
  constant FragmentUniforms &uniforms [[ buffer(kFragmentUniformsIndex) ]],
  constant Light* lights [[ buffer(kLightIndex) ]],
  constant TextureIndices& textureIndices [[ buffer(kTextureIndicesIndex) ]],
  constant TextureInfo* textureInfoBuffer [[ buffer(kTextureInfoIndex) ]],
  texture2d_array<float> textureArray [[ texture(kTextureArrayIndex) ]],
  depth2d<float> shadowTexture [[ texture(kTextureShadowIndex) ]]
) {
  constexpr sampler textureSampler(
    filter::linear,
    mip_filter::linear,
    max_anisotropy(8),
    address::repeat);
  if (!is_null_texture(textureArray)) {
    if (textureIndices.diffuseTextureIndex < 0 || textureIndices.diffuseTextureIndex >= int(textureArray.get_array_size())) {
      float3 color = float3(0.5, 0.5, 0.5);
      if (!is_null_texture(shadowTexture)) {
        color *= calculateShadow(in.shadowPosition, shadowTexture);
      }
      return float4(color, 1.0f);//float4(1.0, 0.0, 0.0, 1.0); // Return red color for invalid texture index
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
    float3 specularColor = 0;
    float3 finalDiffuseColor = 0;
    float3 F0 = mix(0.04, material.baseColor, material.metallic);

    for (uint i = 0; i < uniforms.lightCount; i++) {
      Light light = lights[i];
      float3 lightDirection = normalize(light.position);
//      float3 F0 = mix(0.04, material.baseColor, material.metallic);
      specularColor += saturate(
        computeSpecular(normal, viewDirection, lightDirection, material.roughness, F0)
      );
      finalDiffuseColor += saturate(
        computeDiffuse(material, normal,lightDirection) * light.color
      );
    }

    if (!is_null_texture(shadowTexture)) {
      finalDiffuseColor *= calculateShadow(in.shadowPosition, shadowTexture);
    }
    return float4(finalDiffuseColor + specularColor, 1.0f);
  } else {
    float3 color = float3(0.5, 0.5, 0.5);
    if (!is_null_texture(shadowTexture)) {
      color *= calculateShadow(in.shadowPosition, shadowTexture);
    }
    return float4(color, 1.0f);
  }
}
