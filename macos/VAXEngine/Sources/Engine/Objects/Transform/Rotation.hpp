//
// Created by Alexander Lakhonin on 27.02.2025.
//

#ifndef Rotation_hpp
#define Rotation_hpp

#include <stdio.h>
#include <simd/simd.h>

struct Rotation {
public:
  simd_float3 angles;

  Rotation(simd_float3 angles = simd_make_float3(0.0f, 0.0f, 0.0f)): angles(angles) { };

  matrix_float4x4 rotationMatrix() const;
};

#endif /* Rotation_hpp */
