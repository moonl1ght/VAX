//
// Created by Alexander Lakhonin on 27.02.2025.
//

#include "Rotation.hpp"
#include "AAPLMathUtilities.h"

matrix_float4x4 Rotation::rotationMatrix() const {
  auto YXZ = Rotation::yxzRotationMatrix(angles);
  auto quaternion = simd_quaternion(YXZ);
  return matrix4x4_from_quaternion(quaternion.vector);
}

matrix_float4x4 Rotation::yxzRotationMatrix(vector_float3 angles) {
  auto rotationX = matrix4x4_rotation(angles[0], (vector_float3){1, 0, 0});
  auto rotationY = matrix4x4_rotation(angles[1], (vector_float3){0, 1, 0});
  auto rotationZ = matrix4x4_rotation(angles[2], (vector_float3){0, 0, 1});
  return matrix_multiply(matrix_multiply(rotationY, rotationX), rotationZ);
}
