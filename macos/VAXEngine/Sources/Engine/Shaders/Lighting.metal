//
// Created by Alexander Lakhonin on 07.03.2025.
//

#include <metal_stdlib>
using namespace metal;

#import "Lighting.h"

constant float pi = 3.1415926535897932384626433832795;

float3 phongLighting(
  float3 normal,
  float3 position,
  constant FragmentUniforms &uniforms,
  constant Light *lights,
  float3 baseColor)
{
  float3 diffuseColor = baseColor * 0.5; // self color as an ambient
  float3 ambientColor = 0;
  float3 specularColor = 0;

  float materialShininess = 32;
  float3 materialSpecularColor = float3(1, 1, 1);

  for (uint i = 0; i < uniforms.lightCount; i++) {
    Light light = lights[i];
    switch (light.type) {
      case Sunlight: {
        float3 lightDirection = normalize(-light.position);
        float diffuseIntensity = saturate(-dot(lightDirection, normal));
        diffuseColor += light.color * baseColor * diffuseIntensity;
        if (diffuseIntensity > 0) {
          float3 reflection = reflect(lightDirection, normal);
          float3 viewDirection = normalize(uniforms.cameraPosition);
          float specularIntensity = pow(saturate(dot(reflection, viewDirection)), materialShininess);
          specularColor += light.specularColor * materialSpecularColor * specularIntensity;
        }
        break;
      }
      case Pointlight: {
        float d = distance(light.position, position);
        float3 lightDirection = normalize(light.position - position);
        float attenuation = 1.0 / (light.attenuation.x +
                                   light.attenuation.y * d + light.attenuation.z * d * d);

        float diffuseIntensity = saturate(dot(lightDirection, normal));
        float3 color = light.color * baseColor * diffuseIntensity;
        color *= attenuation;
        diffuseColor += color;
        break;
      }
      case Spotlight: {
        float d = distance(light.position, position);
        float3 lightDirection = normalize(light.position - position);
        float3 coneDirection = normalize(light.coneDirection);
        float spotResult = dot(lightDirection, -coneDirection);
        if (spotResult > cos(light.coneAngle)) {
          float attenuation = 1.0 / (light.attenuation.x +
                                     light.attenuation.y * d + light.attenuation.z * d * d);
          attenuation *= pow(spotResult, light.coneAttenuation);
          float diffuseIntensity = saturate(dot(lightDirection, normal));
          float3 color = light.color * baseColor * diffuseIntensity;
          color *= attenuation;
          diffuseColor += color;
        }
        break;
      }
      case Ambientlight: {
        ambientColor += light.color;
        break;
      }
      case unused: {
        break;
      }
    }
  }
  return diffuseColor + specularColor + ambientColor;
}

float G1V(float nDotV, float k)
{
  return 1.0f / (nDotV * (1.0f - k) + k);
}

// specular optimized-ggx
// AUTHOR John Hable. Released into the public domain
float3 computeSpecular(
  float3 normal,
  float3 viewDirection,
  float3 lightDirection,
  float roughness,
  float3 F0
) {
  float alpha = roughness * roughness;
  float3 halfVector = normalize(viewDirection + lightDirection);
  float nDotL = saturate(dot(normal, lightDirection));
  float nDotV = saturate(dot(normal, viewDirection));
  float nDotH = saturate(dot(normal, halfVector));
  float lDotH = saturate(dot(lightDirection, halfVector));

  float3 F;
  float D, vis;

  // Distribution
  float alphaSqr = alpha * alpha;
  float pi = 3.14159f;
  float denom = nDotH * nDotH * (alphaSqr - 1.0) + 1.0f;
  D = alphaSqr / (pi * denom * denom);

  // Fresnel
  float lDotH5 = pow(1.0 - lDotH, 5);
  F = F0 + (1.0 - F0) * lDotH5;

  // V
  float k = alpha / 2.0f;
  vis = G1V(nDotL, k) * G1V(nDotV, k);

  float3 specular = nDotL * D * F * vis;
  return specular;
}

// diffuse
float3 computeDiffuse(
                      Material material,
                      float3 normal,
                      float3 lightDirection)
{
  float nDotL = saturate(dot(normal, lightDirection));
  float3 diffuse = float3(((1.0/pi) * material.baseColor) * (1.0 - material.metallic));
  diffuse = float3(material.baseColor) * (1.0 - material.metallic);
  return diffuse * nDotL * material.ambientOcclusion;
}
