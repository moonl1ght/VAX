//
// Created by Alexander Lakhonin on 07.03.2025.
//

#ifndef Lighting_h
#define Lighting_h

#include "ShaderTypes.h"

float3 phongLighting(
  float3 normal,
  float3 position,
  constant FragmentUniforms &params,
  constant Light *lights,
  float3 baseColor);

float3 computeSpecular(
  float3 normal,
  float3 viewDirection,
  float3 lightDirection,
  float roughness,
  float3 F0);

float3 computeDiffuse(
  Material material,
  float3 normal,
  float3 lightDirection);

#endif /* Lighting_h */
