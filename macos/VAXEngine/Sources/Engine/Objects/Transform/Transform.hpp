//
// Created by Alexander Lakhonin on 27.02.2025.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <stdio.h>
#include <simd/simd.h>

#include "Rotation.hpp"
#include "AAPLMathUtilities.h"

struct Transform {
public:
  simd_float3 position;
  Rotation rotation;
  simd_float3 scale;

  Transform()
  : position((simd_float3){0.0f, 0.0f, 0.0f})
  , rotation(Rotation())
  , scale((simd_float3){1.0f, 1.0f, 1.0f})
  { };

  inline matrix_float4x4 modelMatrix() const {
    return matrix_multiply(
      matrix_multiply(matrix4x4_translation(position), rotation.rotationMatrix()),
      matrix4x4_scale(scale)
    );
  }
};

#endif /* Transform_hpp */
