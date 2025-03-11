//
// Created by Alexander Lakhonin on 07.03.2025.
//

#ifndef Lighting_h
#define Lighting_h

#include "ShaderTypes.h"

float3 phongLighting(float3 normal,
                     float3 position,
                     constant FragmentUniforms &params,
                     constant Light *lights,
                     float3 baseColor);

#endif /* Lighting_h */
