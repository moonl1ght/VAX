//
// Created by Alexander Lakhonin on 07.03.2025.
//

#include <metal_stdlib>
using namespace metal;

#import "Lighting.h"

float3 phongLighting(float3 normal,
                     float3 position,
                     constant FragmentUniforms &params,
                     constant Light *lights,
                     float3 baseColor)
{
  float3 diffuseColor = 0;
  float3 ambientColor = 0;
  float3 specularColor = 0;

  float materialShininess = 32;
  float3 materialSpecularColor = float3(1, 1, 1);

  for (uint i = 0; i < params.lightCount; i++) {
    Light light = lights[i];
    switch (light.type) {
      case Sunlight: {
        float3 lightDirection = normalize(-light.position);
        float diffuseIntensity = saturate(-dot(lightDirection, normal));
        diffuseColor += light.color * baseColor * diffuseIntensity;
        if (diffuseIntensity > 0) {
          float3 reflection = reflect(lightDirection, normal);
          float3 viewDirection = normalize(params.cameraPosition);
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
