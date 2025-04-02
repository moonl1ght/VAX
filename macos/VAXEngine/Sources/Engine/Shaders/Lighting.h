//
// Created by Alexander Lakhonin on 07.03.2025.
//

#ifndef Lighting_h
#define Lighting_h

#include "headers/ShadersData.h"
#include "headers/Light.h"
#include "headers/Material.h"

float3 phongLighting(
  simd_float3 normal,
  simd_float3 position,
  constant FragmentUniforms &params,
  constant Light *lights,
  simd_float3 baseColor);

float3 computeSpecular(
  simd_float3 normal,
  simd_float3 viewDirection,
  simd_float3 lightDirection,
  float roughness,
  simd_float3 F0);

float3 computeDiffuse(
  Material material,
  simd_float3 normal,
  simd_float3 lightDirection);

float calculateShadow(float4 shadowPosition, depth2d<float> shadowTexture);

#endif /* Lighting_h */
