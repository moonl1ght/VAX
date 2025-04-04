//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include "Camera.hpp"

#include <algorithm>
#include <iostream>

matrix_float4x4 Camera::viewMatrix() const {
  if (target.x == transform.position.x && target.y == transform.position.y && target.z == transform.position.z) {
    return simd_inverse(matrix_multiply(matrix4x4_translation(target), transform.rotation.rotationMatrix()));
  } else {
    return matrix_look_at_left_hand(transform.position, target, (vector_float3){0.0f, 1.0f, 0.0f});
  }
}

matrix_float4x4 Camera::projectionMatrix() const {
  switch (projection) {
    case Projection::orthographic: {
      float left = -viewSize * aspectRatio * 0.5;
      float right = left + viewSize * aspectRatio;
      float top = viewSize * 0.5;
      float bottom = top - viewSize;
      return matrix_ortho_left_hand(left, right, -top, -bottom, nearPlane, farPlane);
      break;
    }
    case Projection::perspective:
      return matrix_perspective_left_hand(fov, aspectRatio, nearPlane, farPlane);
      break;
  }
}

void Camera::precalculateMatrices() {
  savedViewMatrix = viewMatrix();
  savedProjectionMatrix = projectionMatrix();
}

void Camera::rotate(simd_float2 delta) {

  transform.rotation.setY(transform.rotation.y() + delta.x * 0.05);
  transform.rotation.setX(transform.rotation.x() + (-delta.y) * 0.05);
  transform.rotation.setX(std::max((float)-M_PI_2+0.01f, std::min(transform.rotation.x(), (float)M_PI_2-0.01f)));
  auto distanceToOrigin = simd_length(transform.position);
  auto rotationX = matrix4x4_rotation(transform.rotation.angles[0], (vector_float3){1, 0, 0});
  auto rotationY = matrix4x4_rotation(transform.rotation.angles[1], (vector_float3){0, 1, 0});
  auto rotateMatrix = matrix_multiply(rotationY, rotationX);
  auto distanceVector = simd_make_float4(0.0f, 0.0f, distanceToOrigin, 0.0f);
  auto rotatedVector = matrix_multiply(rotateMatrix, distanceVector);
  transform.position = rotatedVector.xyz;//simd_make_float3(rotatedVector.xy, transform.position.z);
}

void Camera::setPosition(simd_float3 position) {
  transform.position = position;
  updateRotationForPosition();
}

void Camera::updateRotationForPosition() {
  auto x = transform.position.x;
  auto y = transform.position.y;
  auto z = transform.position.z;
  transform.rotation.setY(std::atan2(x, z));
  bool isNegative = z < 0 && y < 0;
  auto xr = z < 0 ? std::atan2(y, z) + (isNegative ? (float)M_PI : -(float)M_PI): -std::atan2(y, z);
  transform.rotation.setX(xr);
}
