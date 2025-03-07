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
  matrix_float4x4 yxzRotationMatrix(vector_float3 axis) const;

  inline float x() const { return angles[0]; };
  inline float y() const { return angles[1]; };
  inline float z() const { return angles[2]; };

  inline void setX(float x) { angles[0] = x; };
  inline void setY(float y) { angles[1] = y; };
  inline void setZ(float z) { angles[2] = z; };
};

#endif /* Rotation_hpp */
