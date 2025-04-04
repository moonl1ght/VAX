//
// Created by Alexander Lakhonin on 03.04.2025.
//

#ifndef FrustumPoints_h
#define FrustumPoints_h

struct FrustumPoints {
  matrix_float4x4 viewMatrix = matrix4x4_identity();
  simd_float3 upperLeft;
  simd_float3 upperRight;
  simd_float3 lowerRight;
  simd_float3 lowerLeft;

  static FrustumPoints calculatePlanePoints(
    matrix_float4x4 matrix,
    float halfWidth,
    float halfHeight,
    float distance,
    simd_float3 position
  ) {
    simd_float3 forwardVector = {matrix.columns[0].z, matrix.columns[1].z, matrix.columns[2].z};
    simd_float3 rightVector = {matrix.columns[0].x, matrix.columns[1].x, matrix.columns[2].x};
    auto upVector = simd_cross(forwardVector, rightVector);
    auto centerPoint = position + forwardVector * distance;
    auto moveRightBy = rightVector * halfWidth;
    auto moveDownBy = upVector * halfHeight;
    return {
      .viewMatrix = matrix,
      .upperLeft = centerPoint - moveRightBy + moveDownBy,
      .upperRight = centerPoint + moveRightBy + moveDownBy,
      .lowerRight = centerPoint + moveRightBy - moveDownBy,
      .lowerLeft = centerPoint - moveRightBy - moveDownBy
    };
  };
};

#endif /* FrustumPoints_h */
