//
// Created by Alexander Lakhonin on 02.04.2025.
//

#ifndef Material_h
#define Material_h

typedef struct {
  vector_float3 baseColor;
  vector_float3 specularColor;
  float roughness;
  float metallic;
  float ambientOcclusion;
  float shininess;
} Material;

#endif /* Material_h */
