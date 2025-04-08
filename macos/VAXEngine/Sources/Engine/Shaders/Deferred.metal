//
// Created by Alexander Lakhonin on 08.04.2025.
//

#include <metal_stdlib>
#include "headers/AttributesAndIndices.h"
#include "headers/VertexData.h"
#include "headers/Material.h"
#include "Lighting.h"

using namespace metal;

struct GBufferOut {
  float4 albedo [[ color(kRenderTargetAlbedo) ]];
  float4 normal [[ color(kRenderTargetNormal) ]];
  float4 position [[ color(kRenderTargetPosition) ]];
};

fragment GBufferOut fragment_gBuffer(
  VertexOut in [[stage_in]],
  constant TextureIndices& textureIndices [[ buffer(kTextureIndicesIndex) ]],
  constant TextureInfo* textureInfoBuffer [[ buffer(kTextureInfoIndex) ]],
  texture2d_array<float> textureArray [[ texture(kTextureArrayIndex) ]],
  depth2d<float> shadowTexture [[ texture(kTextureShadowIndex) ]]
) {
  Material material;
  GBufferOut out;

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
    out.albedo = float4(diffuseColor, 1.0);
  } else {
    out.albedo = float4(0.5, 0.5, 0.5, 1.0);
  }

  out.albedo.a = calculateShadow(in.shadowPosition, shadowTexture);

  out.normal = float4(normalize(in.worldNormal), 1.0);
  out.position = float4(in.fragmentWorldPosition.xyz, 1.0);
  return out;
}

constant float3 vertices[6] = {
  float3(-1,  1,  0),    // triangle 1
  float3( 1, -1,  0),
  float3(-1, -1,  0),
  float3(-1,  1,  0),    // triangle 2
  float3( 1,  1,  0),
  float3( 1, -1,  0)
};

vertex VertexOut vertex_quad(uint vertexID [[vertex_id]])
{
  VertexOut out {
    .position = float4(vertices[vertexID], 1)
  };
  return out;
}

fragment float4 fragment_deferredSun(
                                     VertexOut in [[stage_in]],
                                     constant FragmentUniforms &uniforms [[buffer(kFragmentUniformsIndex)]],
                                     constant Light *lights [[buffer(kLightIndex)]],
                                     texture2d<float> albedoTexture [[ texture(kBaseColorTextureIndex) ]],
                                     texture2d<float> normalTexture [[ texture(kNormalTextureIndex) ]],
                                     texture2d<float> positionTexture [[ texture(kPositionTextureIndex) ]])
{
  uint2 coord = uint2(in.position.xy);
  float4 albedo = albedoTexture.read(coord);
  float3 normal = normalTexture.read(coord).xyz;
  float3 position = positionTexture.read(coord).xyz;
  Material material {
    .baseColor = albedo.xyz,
    .specularColor = float3(0),
    .shininess = 500
  };
  float3 color = phongLighting(normal, position, uniforms, lights, material.baseColor);

  color *= albedo.a;
  return float4(color, 1);
}
